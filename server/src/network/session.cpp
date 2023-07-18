#include "network/session.h"

websocket_session::websocket_session(tcp::socket socket, std::shared_ptr<shared_state> const& state)
  : ws(std::move(socket)), state(state) {}

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
                std::bind(buffer, [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                  sp->on_read(ec, bytes);
                }));
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

  ws.async_write(*queue.front(), 
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
    ws.async_write(*queue.front(), 
                   [sp = shared_from_this()](error_code ec, std::size_t bytes) {
                     sp->on_write(ec, bytes);
                   });
}