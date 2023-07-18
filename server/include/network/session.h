#ifndef SPACE_NETWORK_SESSION_H
#define SPACE_NETWORK_SESSION_H

#include "network/utils.h"
#include "network/shared_state.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <queue>
#include <iostream>

class shared_state;

class websocket_session : public std::enable_shared_from_this<websocket_session> {
private:
  beast::flat_buffer buffer;
  websocket::stream<tcp::socket> ws;
  std::shared_ptr<shared_state> state;
  std::queue<std::shared_ptr<std::string const>> queue;

  void fail(error_code ec, char const* what);
  void on_accept(error_code ec);
  void on_read(error_code ec, std::size_t bytes_transferred);
  void on_write(error_code ec, std::size_t bytes_transferred);

public:
  websocket_session(tcp::socket socket, std::shared_ptr<shared_state> const& state);
  ~websocket_session();

  template<class Body, class Allocator>
  void run(http::request<Body, http::basic_fields<Allocator>> req);

  void send(std::shared_ptr<std::string const> const& ss);
};

template<class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator>> req) {
  ws.async_accept(req, std::bind(&websocket_session::on_accept, shared_from_this(), std::placeholders::_1));
}

#endif //SPACE_NETWORK_SESSION_H