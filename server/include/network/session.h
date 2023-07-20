#ifndef SPACE_NETWORK_SESSION_H
#define SPACE_NETWORK_SESSION_H

#include "network/utils.h"
#include "network/shared_state.h"
#include "handler/handler.h"
#include "common.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <queue>
#include <vector>
#include <iostream>
#include <string_view>

class shared_state;

class websocket_session;

class http_session : public std::enable_shared_from_this<http_session> {
private:
  tcp::socket socket;
  beast::flat_buffer buffer;
  std::shared_ptr<shared_state> state;
  http::request<http::string_body> req;

  void fail(error_code ec, char const* what);
  void on_read(error_code ec, std::size_t);
  void on_write(error_code ec, std::size_t, bool close);
  bool handle_custom_route();

  template<class Response>
  void async_write(Response resp);

public:
  http_session(tcp::socket socket, std::shared_ptr<shared_state> const& state);

  void run();
};

template<class Response>
void http_session::async_write(Response resp) {
  auto sp = std::make_shared<Response>(std::move(resp));
  auto self = shared_from_this();

  http::async_write(socket, *sp, [self, sp](error_code ec, std::size_t bytes) {
    self->on_write(ec, bytes, sp->need_eof());
  });
}


class websocket_session : public std::enable_shared_from_this<websocket_session> {
private:
  beast::flat_buffer buffer;
  std::unique_ptr<websocket::stream<tcp::socket>> ws;
  std::shared_ptr<shared_state> state;
  std::queue<std::shared_ptr<std::vector<uint8_t> const>> queue;

  handler_sptr handler;
  std::string channel;

  void fail(error_code ec, char const* what);
  void on_accept(error_code ec);
  void on_read(error_code ec, std::size_t bytes_transferred);
  void on_write(error_code ec, std::size_t bytes_transferred);

public:
  websocket_session(tcp::socket socket, std::shared_ptr<shared_state> const& state);
  ~websocket_session();

  template<class Body, class Allocator>
  void run(http::request<Body, http::basic_fields<Allocator>> req, std::string path);

  void send(std::shared_ptr<std::vector<uint8_t> const> const& ss);

  std::string const& get_channel() const noexcept { return channel; }
};

template<class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator>> req, std::string path) {
  this->channel = path;
  handler = state->get_handler(channel, shared_state::WEBSOCKET);

  if (!handler || !handler->on_open(req))
    return;

  ws->async_accept(req, std::bind(&websocket_session::on_accept, shared_from_this(), std::placeholders::_1));
}

#endif //SPACE_NETWORK_SESSION_H