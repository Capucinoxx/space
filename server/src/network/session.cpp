#include "network/session.h"

websocket_session::websocket_session(tcp::socket socket, std::shared_ptr<shared_state> const& state)
  : ws(std::move(socket)), state(state) {
    std::cout << "websocket_session::websocket_session\n";
  }

websocket_session::~websocket_session() {
  state->leave(*this);
}

void websocket_session::fail(error_code ec, char const* what) {
  if (ec == net::error::operation_aborted || ec == websocket::error::closed)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

void websocket_session::on_accept(error_code ec) {
  if (ec)
    return fail(ec, "accept");

  state->join(*this);

  ws.async_read(buffer, 
                [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                  sp->on_read(ec, bytes);
                });
}

void websocket_session::on_read(error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "read");

  state->send(beast::buffers_to_string(buffer.data()));

  buffer.consume(buffer.size());

  ws.async_read(buffer, 
                [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                  sp->on_read(ec, bytes);
                });
}

void websocket_session::send(std::shared_ptr<std::string const> const& ss) {
  queue.push(ss);

  if (queue.size() > 1)
    return;

  ws.async_write(net::buffer(*queue.front()), 
                 [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                   sp->on_write(ec, bytes);
                 });
}

void websocket_session::on_write(error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "write");

  queue.pop();

  if (!queue.empty())
    ws.async_write(net::buffer(*queue.front()), 
                   [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                     sp->on_write(ec, bytes);
                   });
}

// =================================================================================================


beast::string_view mime_type(beast::string_view path) {
  using beast::iequals;
  auto const ext = [&path]{
    auto const pos = path.rfind(".");
    if(pos == beast::string_view::npos)
      return beast::string_view{};
    return path.substr(pos);
  }();
  
  if(iequals(ext, ".js"))   return "application/javascript";
  if(iequals(ext, ".html")) return "text/html";
  if(iequals(ext, ".css"))  return "text/css";
  if(iequals(ext, ".txt"))  return "text/plain";
  if(iequals(ext, ".png"))  return "image/png";
  if(iequals(ext, ".jpeg")) return "image/jpeg";
  if(iequals(ext, ".jpg"))  return "image/jpeg";
  if(iequals(ext, ".gif"))  return "image/gif";
  if(iequals(ext, ".svg"))  return "image/svg+xml";
  return "application/text";
}

std::string path_cat(beast::string_view base, beast::string_view path) {
  if (base.empty())
    return std::string(path);
  
  std::string result(base);

  char constexpr path_separator = '/';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
  return result;
}

template<class Body, class Allocator, class Send>
void handle_request(beast::string_view doc_root, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
  auto const bad_request = [&req](beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, "Space");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  auto const not_found = [&req](beast::string_view target) {
    http::response<http::string_body> res{ http::status::not_found, req.version() };
    res.set(http::field::server, "Space");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  auto const server_error = [&req](beast::string_view what) {
    http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
    res.set(http::field::server, "Space");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
  };

  if (req.method() != http::verb::get && req.method() != http::verb::head)
    return send(bad_request("Unknown HTTP-method"));

  if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos)
    return send(bad_request("Illegal request-target"));

  std::string path = path_cat(doc_root, req.target());
  if (req.target().back() == '/')
    path.append("index.html");

  error_code ec;
  http::file_body::value_type body;
  body.open(path.c_str(), beast::file_mode::scan, ec);

  if (ec == beast::errc::no_such_file_or_directory)
    return send(not_found(req.target()));

  if (ec)
    return send(server_error(ec.message()));

  auto const size = body.size();
  if (req.method() == http::verb::head) {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "SPACE");
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
  }

  http::response<http::file_body> res{std::piecewise_construct, 
                                      std::make_tuple(std::move(body)), 
                                      std::make_tuple(http::status::ok, req.version())};
  
  res.set(http::field::server, "SPACE");
  res.set(http::field::content_type, mime_type(path));
  res.content_length(size);
  res.keep_alive(req.keep_alive());
  return send(std::move(res));
}

http_session::http_session(tcp::socket socket, std::shared_ptr<shared_state> const& state)
  : socket(std::move(socket)), state(state) {}

void http_session::run() {
  beast::http::async_read(socket, buffer, req, 
                          [self = shared_from_this()](error_code ec, std::size_t bytes) {
                            self->on_read(ec, bytes);
                          });
}

void http_session::fail(error_code ec, char const* what) {
  if (ec == net::error::operation_aborted)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

void http_session::on_read(error_code ec, std::size_t) {
  if (ec == http::error::end_of_stream) {
    socket.shutdown(tcp::socket::shutdown_send, ec);
    return;
  }

  if (ec)
    return fail(ec, "read");

  if (websocket::is_upgrade(req)) {
    std::make_shared<websocket_session>(std::move(socket), state)->run(std::move(req));
    return;
  }
  
  if (auto [status, body] = state->handle_http_request(req); status != http::status::not_found) {
    http::response<http::string_body> res{status, req.version()};
    res.set(http::field::server, "Space");
    res.set(http::field::content_type, "application/octet-stream");
    res.keep_alive(req.keep_alive());
    res.body() = body;
    res.prepare_payload();

    http::async_write(socket, res, [this](error_code ec, std::size_t bytes) {
      this->on_write(ec, bytes, false);
    });
    return;
  }

  handle_request(state->doc_root(), std::move(req), [this](auto&& response) {
    using response_type = typename std::decay<decltype(response)>::type;
    auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

    auto self = shared_from_this();
    http::async_write(this->socket, *sp, [self, sp](error_code ec, std::size_t bytes) {
      self->on_write(ec, bytes, sp->need_eof());
    });
  });
}

void http_session::on_write(error_code ec, std::size_t, bool close) {
  if (ec)
    return fail(ec, "write");

  if (close) {
    socket.shutdown(tcp::socket::shutdown_send, ec);
    return;
  }

  req = {};

  beast::http::async_read(socket, buffer, req, 
                          [self = shared_from_this()](error_code ec, std::size_t bytes) {
                            self->on_read(ec, bytes);
                          });
}
