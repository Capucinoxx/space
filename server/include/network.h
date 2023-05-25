#ifndef SPACE_NETWORK_H
#define SPACE_NETWORK_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class websocket_server {
public:
  using tcp = net::ip::tcp;

private:
  tcp::aceptor acceptor;
  tcp::socket socket;

public:
  websocket_server(net::io_context& ioc, const tcp::endpoint& endpoint)
    : acceptor(ioc, endpoint), socket(ioc) { }

  ~websocket_server() { acceptor.close(); }

private:
  void accept() {
    acceptor.async_accept(socket, [this](beast::error_code ec) {
      if (!ec)
        std::thread(&websocket_server::session, this, std::move(socket)).detach();

      accept();
    });
  }

  void session(tcp::socket socket) {
    try {
      websocket::stream<tcp::socket> ws(std::move(socket));
      ws.accept();

      beast::flat_buffer buffer;
      while(true) {
        ws.read(buffer);
        std::string msg = beast::buffers_to_string(buffer.data());

        ws.write(net::buffer(msg));
        buffer.consume(buffer.size());
      }

    } catch(const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
};

#endif //SPACE_NETWORK_H
