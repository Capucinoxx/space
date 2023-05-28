#ifndef SPACE_NETWORK_H
#define SPACE_NETWORK_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <unordered_set>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

template<uint32_t ROWS, uint32_t COLS>
class Session : public std::enable_shared_from_this<Session<ROWS, COLS>> {
public:
  using tcp = net::ip::tcp;
  using stream_socket = websocket::stream<tcp::socket>;

private:
  stream_socket ws;
  beast::flat_buffer write_buffer;
  beast::flat_buffer read_buffer;

public:
  explicit Session(tcp::socket&& socket) : ws(std::move(socket)) {}

  void run() {
    net::dispatch(ws.get_executor(),
                  beast::bind_front_handler(
                      &Session<ROWS, COLS>::on_run,
                      this->shared_from_this()));
  }

  void write(const std::vector<uint8_t>& data) {
    ws.async_write(asio::buffer(data), beast::bind_front_handler(&Session<ROWS, COLS>::on_write, this->shared_from_this()));
  }

private:
  void on_run() {
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
      res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " ws-server");
    }));

    ws.async_accept(beast::bind_front_handler(&Session::on_accept, this->shared_from_this()));
  }

  void on_accept(beast::error_code ec) {
    if (!ec)
      do_read();
  }

  void do_read() {
    ws.async_read(read_buffer, beast::bind_front_handler(&Session::on_read, this->shared_from_this()));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec == websocket::error::closed)
      return;

    if (!ec) {
      auto text = read_buffer.data().data();
      std::cout << "READ: " << static_cast<char *>(text) << std::endl;
      read_buffer.consume(read_buffer.size());
    }

    do_read();
  }

  void on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return;
  }
};

template<uint32_t ROWS, uint32_t COLS>
class Listener : public std::enable_shared_from_this<Listener<ROWS, COLS>> {
public:
  using tcp = net::ip::tcp;

private:
  net::io_context& ioc;
  tcp::acceptor acceptor;
  std::unordered_set<std::shared_ptr<Session<ROWS, COLS>>> sessions;

public:
  Listener(net::io_context& ioc, tcp::endpoint endpoint) : ioc(ioc), acceptor(ioc) {
    beast::error_code ec;

    acceptor.open(endpoint.protocol(), ec);
    if (ec) return;

    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) return;

    acceptor.bind(endpoint, ec);
    if (ec) return;

    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec) return;
  }

  void run() {
    do_accept();
  }

  void broadcast(const std::vector<uint8_t>& data) {
    for (const auto& session : sessions)
      session->write(data);
  }

private:
  void do_accept() {
    acceptor.async_accept(
        net::make_strand(ioc),
        beast::bind_front_handler(
            &Listener::on_accept,
            this->shared_from_this()));
  }

  void on_accept(beast::error_code ec, tcp::socket socket) {
    if (!ec) {
      auto session = std::make_shared<Session<ROWS, COLS>>(std::move(socket));
      sessions.insert(session);
      session->run();
    }

    do_accept();
  }
};

template<uint32_t ROWS, uint32_t COLS>
class Server {
public:
  using tcp = net::ip::tcp;

private:
  net::io_context ioc;
  tcp::endpoint endpoint;
  int threads;
  std::shared_ptr<Listener<ROWS, COLS>> listener;

public:
  Server(const std::string& address, unsigned short port, int threads)
    : threads{ threads }, ioc{ threads }, endpoint{ net::ip::make_address(address), port } {}

  void listen() {
    listener = std::make_shared<Listener<ROWS, COLS>>(ioc, endpoint);
    listener->run();

    std::vector<std::thread> v;
    v.reserve(threads - 1);

    for (auto i = threads - 1; i > 0; --i)
      v.emplace_back([&]{ ioc.run(); });

    ioc.run();
  }

  void broadcast(const std::vector<uint8_t>& data) {
    listener->broadcast(data);
  }
};

#endif //SPACE_NETWORK_H
