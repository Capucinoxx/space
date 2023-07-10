#ifndef SPACE_NETWORK_H
#define SPACE_NETWORK_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <algorithm>
#include <variant>
#include <string_view>

#include "game_state.h"
#include "player.h"
#include "utils.h"

std::shared_mutex ws_connections_mu;

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

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


bool is_post(http::request<http::string_body>& req) noexcept { return req.method() == http::verb::post; }
bool is_get(http::request<http::string_body>& req) noexcept  { return req.method() == http::verb::get; }

void prepare_http_response(http::request<http::string_body>& req, http::response<http::string_body>& resp, const std::string& body) {
  resp.set(http::field::server, "Space");
  resp.set(http::field::content_type, "text/plain");
  resp.set(http::field::access_control_allow_origin, "*");
  resp.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
  resp.set(http::field::access_control_allow_headers, "Content-Type");
  resp.set(http::field::access_control_max_age, "86400");
  resp.set(http::field::content_length, "0");
  resp.keep_alive(req.keep_alive());
  resp.body() = body;
  resp.prepare_payload();
};

std::string retrieve_field(const std::string& body, const std::string& field) {
    std::size_t pos = body.find(field);
    if (pos == std::string::npos)
      return "";

    std::size_t start = pos + field.size() + 1;
    std::size_t end = body.find('&', start);
    if (end == std::string::npos)
      end = body.size();

    return body.substr(start, end - start);
  };

class WebSocketHandler {
public:
  using tcp = net::ip::tcp;
  using ws_stream_ptr = std::shared_ptr<websocket::stream<tcp::socket>>;
  using http_request = http::request<http::string_body>&;

  virtual bool on_open(ws_stream_ptr ws, http_request req) = 0;
  virtual void on_message(const std::string& message) = 0;
  virtual void on_close() = 0;

  virtual bool handle_message() = 0;
};

class Server {
public:
  using tcp = net::ip::tcp;

  using http_request = http::request<http::string_body>&;
  using http_response = http::response<http::string_body>&;
  using http_handler = std::function<std::pair<http::status, std::string>(http_request)>;

  using ws_stream_ptr = std::shared_ptr<websocket::stream<tcp::socket>>;
  using ws_handler = std::function<std::unique_ptr<WebSocketHandler>()>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> http_endpoints;
  std::map<std::string, ws_handler>   ws_endpoints;

  std::unordered_map<std::string, std::unordered_set<ws_stream_ptr>> ws_connections{};
  std::vector<std::thread> thread_pool;


public:
  explicit Server(short unsigned int port, std::size_t thread_pool_size = 4)
    : acceptor(ioc, { tcp::v4(), port }), thread_pool(thread_pool_size) {
      ws_connections.clear();
    };

  ~Server() {
    for (auto& connections : ws_connections)
      for (auto& ws : connections.second)
        ws->close(websocket::close_code::normal);

    ioc.stop();

    for (auto& th : thread_pool)
      th.join();
  }

  void run() {
    start_accept();

    for (std::size_t i = 0; i != thread_pool.size(); ++i)
      thread_pool[i] = std::thread([this]() { ioc.run(); });

    for (std::size_t i = 0; i != thread_pool.size(); ++i)
      thread_pool[i].join();
  }

  void add_http_endpoint(const std::string& path, http_handler handler) {
    http_endpoints[path] = handler;
  }

  void add_ws_endpoint(const std::string& path, ws_handler handler) {
    ws_endpoints[path] = handler;
  }

  void broadcast_websocket_message(const std::string& channel, const std::vector<uint8_t>& message) {
    std::shared_lock<std::shared_mutex> lock(ws_connections_mu);

    auto it = ws_connections.find(channel);
    if (it == ws_connections.end())
      return;

    for (auto& ws : it->second) {
      if (ws->is_open()) {
        ws->binary(true);
        ws->async_write(net::buffer(message), [](beast::error_code ec, std::size_t) {});
      }
    }
  }

private:
  void start_accept() {
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(ioc);
    acceptor.async_accept(*socket, [this, socket](beast::error_code ec) {
      if (!ec)
        std::make_shared<Connection>(std::move(*socket), std::ref(*this))->run();
      start_accept();
    });
  }

  class Connection : public std::enable_shared_from_this<Connection> {
  private:
    tcp::socket socket;
    beast::flat_buffer buffer;
    http::request<http::string_body> request;

    Server& server;
    std::unique_ptr<WebSocketHandler> handler;

  public:
    Connection(tcp::socket socket, Server& server)
      : socket(std::move(socket)), server(server) { }

    void run() {
      read_request();
    }

  private:
    void read_request() {
      http::async_read(socket, buffer, request, [this, self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (!ec)
          handle_request();
      });
    }

    void handle_request() {
      if (websocket::is_upgrade(request)) {
        upgrade_websocket();
        return;
      }

      http::status status;
      std::string body;  

      auto it = server.http_endpoints.find(request.target().to_string());
      if (it != server.http_endpoints.end()) {
        auto resp = it->second(request);
        status = resp.first;
        body = resp.second;

        http::response<http::string_body> response(status, request.version());
      prepare_http_response(request, response, body);

      beast::error_code ec;
      http::write(socket, std::move(response), ec);
      socket.shutdown(tcp::socket::shutdown_send, ec);
      } else {
        handle_http_request();
      }
    }

    void upgrade_websocket() {
      auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
      ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
      ws->async_accept(request, [this, ws, self = shared_from_this()](beast::error_code ec) {
        if (!ec)
          handle_websocket_upgrade(ws);
      });
    }

    void handle_http_request() {
      auto const bad_request = [this](beast::string_view why) {
        http::response<http::string_body> res{ http::status::bad_request, request.version() };
        res.set(http::field::server, "Space");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
      };

      auto const not_found = [this](beast::string_view target) {
        http::response<http::string_body> res{ http::status::not_found, request.version() };
        res.set(http::field::server, "Space");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
      };

      auto const server_error = [this](beast::string_view what) {
        http::response<http::string_body> res{ http::status::internal_server_error, request.version() };
        res.set(http::field::server, "Space");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
      };

      auto const write = [this](auto&& response) {
        beast::error_code ec;
        http::write(socket, response, ec);
        socket.shutdown(tcp::socket::shutdown_send, ec);
      };

      if (request.method() != http::verb::get && request.method() != http::verb::head)
        return write(bad_request("Unknown HTTP-method"));

      if (request.target().empty() || request.target()[0] != '/' || request.target().find("..") != beast::string_view::npos)
        return write(bad_request("Illegal request-target"));

      std::string path = path_cat("interface", request.target());

      if (request.target().back() == '/')
        path.append("index.html");

      beast::error_code ec;
      http::file_body::value_type body;
      body.open(path.c_str(), beast::file_mode::scan, ec);

      if (ec == beast::errc::no_such_file_or_directory)
        return write(not_found(request.target()));

      if (ec)
        return write(server_error(ec.message()));

      auto const size = body.size();

      if (request.method() == http::verb::head) {
        http::response<http::empty_body> res{ http::status::ok, request.version() };
        res.set(http::field::server, "Space");
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(request.keep_alive());
        return write(res);
      }

      http::response<http::file_body> res{ std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(http::status::ok, request.version()) };
      res.set(http::field::server, "Space");
      res.set(http::field::content_type, mime_type(path));
      res.content_length(size);
      res.keep_alive(request.keep_alive());
      return write(res);
    }

    void handle_websocket_upgrade(ws_stream_ptr ws) {
      auto it = server.ws_endpoints.find(request.target().to_string());
      if (it != server.ws_endpoints.end()) {
        handler = it->second();
        if (handler->on_open(ws, request)) {
          auto channel = std::string(retrieve_channel(request.target().to_string()));

          std::unique_lock<std::shared_mutex> lock(ws_connections_mu);
          if (server.ws_connections.find(channel) == server.ws_connections.end())
            server.ws_connections[channel] = std::unordered_set<ws_stream_ptr>();

          std::cout << "New connection on channel " << channel << std::endl;

          server.ws_connections[channel].insert(ws);

          if (handler->handle_message())
            handle_websocket_message(channel, ws);
        }
      } else
        ws->async_close(websocket::close_code::normal, [](beast::error_code ec) {});
    }

    void handle_websocket_message(const std::string& channel, ws_stream_ptr ws) {
      ws->async_read(buffer, [this, channel, ws, self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (!ec) {
          handler->on_message(beast::buffers_to_string(buffer.data()));
          buffer.consume(buffer.size());
          handle_websocket_message(channel, ws);
        } else {
          std::unique_lock<std::shared_mutex> lock(ws_connections_mu);
          server.ws_connections[channel].erase(ws);
          handler->on_close();
        }
      });
    }
  };
};


class AdminMiddleware {
public:
  std::string admin_password;

  explicit AdminMiddleware(const std::string& admin_password) : admin_password(admin_password) {}

  bool operator()(Server::http_request req) {
    std::string password = req[http::field::authorization].to_string();
    return password == admin_password;
  }
};


class SpectateHandler : public WebSocketHandler {
public:
  bool on_open(ws_stream_ptr ws, http_request req) override { return true; }
  void on_message(const std::string& message) override {}

  void on_close() override {}

  bool handle_message() { return false; }
};



#endif //SPACE_NETWORK_H
