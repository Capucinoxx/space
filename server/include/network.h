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
#include <thread>
#include <mutex>
#include <vector>


namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class WebsocketHandler {
public:
  using tcp = net::ip::tcp;
  using ws_stream_pointer = std::shared_ptr<websocket::stream<tcp::socket>>;
  using http_request = http::request<http::string_body>&;


  virtual void on_open(ws_stream_pointer ws, http_request req) = 0;
  virtual void on_message(const std::string& message) = 0;
};

class Server {
public:
  using tcp = net::ip::tcp;

  using http_request = http::request<http::string_body>&;
  using http_response = http::response<http::string_body>&;
  using http_handler = std::function<void(http_request, http_response)>;

  using ws_stream_pointer = std::shared_ptr<websocket::stream<tcp::socket>>;
  using ws_handler = std::function<std::shared_ptr<WebsocketHandler>()>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> http_endpoints;
  std::map<std::string, ws_handler> ws_endpoints;

  std::unordered_set<ws_stream_pointer> ws_connections;
  std::vector<std::thread> thread_pool;
  std::mutex mu;

public:
  Server(short unsigned int port, std::size_t thread_pool_size = 4)
    : acceptor(ioc, { tcp::v4(), port }), thread_pool(thread_pool_size) {}

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

  void broadcast_websocket_message(const std::vector<uint8_t>& message) {
    for (auto& ws : ws_connections)
      if (ws->is_open())
        ws->async_write(net::buffer(message), [](boost::system::error_code ec, std::size_t) {
          if (ec)
            std::cout << "Error sending message: " << ec.message() << std::endl;
        });
  }

private:
  void start_accept() {
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(ioc);
    acceptor.async_accept(*socket, [this, socket](boost::system::error_code ec) {
      if (!ec) {
        std::make_shared<Connection>(std::move(*socket), *this)->start();
      } else {
        std::cout << "Error accepting connection: " << ec.message() << std::endl;
      }

      start_accept();
    });
  }


  class Connection : public std::enable_shared_from_this<Connection> {
  private:
    tcp::socket socket;
    Server& server;
    http::request<http::string_body> request;


  public:
    Connection(tcp::socket socket, Server& server)
      : socket(std::move(socket)), server(server) {}

    void start() {
      read_request();
    }

  private:
    void read_request() {
      auto buffer = std::make_shared<beast::flat_buffer>();

      http::async_read(socket, *buffer, request,
        [self = shared_from_this(), buffer](boost::system::error_code ec, std::size_t) {
          if (!ec) {
            self->process_request();
          }
        });
    }


    void process_request() {
      auto req = request;
      if (websocket::is_upgrade(req)) {
        auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
        ws->async_accept(request, [self = shared_from_this(), ws, req](boost::system::error_code ec) {
          if (!ec)
            self->process_websocket_request(ws, req);
        });

        return;
      }

      auto it = server.http_endpoints.find(request.target().to_string());
      if (it != server.http_endpoints.end()) {
        http::response<http::string_body> response;
        it->second(request, response);
        write_http_response(response);
        return;
      }

      http::response<http::string_body> response;
      response.result(http::status::not_found);
      response.set(http::field::content_type, "text/plain");
      response.body() = "404 Not Found";
      write_http_response(response);
    }

    void process_websocket_request(ws_stream_pointer ws, http::request<http::string_body> req) {
      add_ws_connection(ws);

      auto it = server.ws_endpoints.find(req.target().to_string());
      if (it != server.ws_endpoints.end()) {
        auto handler = it->second();
        handler->on_open(ws, req);

        read_websocket_message(ws, handler);
      } else {
        ws->async_close(websocket::close_code::normal, [&, ws](boost::system::error_code ec) {
          remove_ws_connection(ws);
        });
      }
    }


    void read_websocket_message(ws_stream_pointer ws, std::shared_ptr<WebsocketHandler> handler) {
      auto buffer = std::make_shared<beast::flat_buffer>();

      ws->async_read(*buffer, [self = shared_from_this(), handler, ws, buffer](boost::system::error_code ec, std::size_t bytes_transferred) {
        if (!ec || ec == boost::asio::error::eof) {
          std::string message(beast::buffers_to_string(buffer->data()));
          buffer->consume(bytes_transferred);

          handler->on_message(message);
          self->read_websocket_message(ws, handler);
        }
      });
    }


    void write_http_response(const http::response<http::string_body>& resp) {
      http::async_write(socket, resp, [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
        if (!ec)
          self->read_request();
      });
    }

    void add_ws_connection(ws_stream_pointer ws) {
      std::lock_guard<std::mutex> lock(server.mu);
      std::cout << &ws << std::endl;
      server.ws_connections.insert(ws);
      std::cout << server.ws_connections.size() << std::endl;
    }

    void remove_ws_connection(ws_stream_pointer ws) {
      std::lock_guard<std::mutex> lock(server.mu);
      server.ws_connections.erase(ws);
      std::cout << "remove " << server.ws_connections.size() << std::endl;
    }
  };

  friend class WebSocketHandler;
};

#endif //SPACE_NETWORK_H