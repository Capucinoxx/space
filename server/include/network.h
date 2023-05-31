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
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class Server {
public:
  using tcp = net::ip::tcp;

  using ws_stream_pointer = std::shared_ptr<websocket::stream<tcp::socket>>;
  using ws_handler        = std::function<void(ws_stream_pointer)>;

  using http_request  = http::request<http::string_body>&;
  using http_response = http::response<http::string_body>&;
  using http_handler  = std::function<void(http_request, http_response)>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> http_endpoints;
  std::map<std::string, ws_handler>   ws_endpoints;

  std::unordered_set<ws_stream_pointer> ws_connections;
  std::vector<std::thread> thread_pool;

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

  void broadcast_websocket_message(const std::string& message) {
    for (auto& ws : ws_connections)
      send_websocket_message(ws, message);
  }

private:
  void start_accept() {
    tcp::socket socket(ioc);

    acceptor.async_accept(socket, [this, &socket](boost::system::error_code ec) {
      if (!ec)
        std::make_shared<Connection>(std::move(socket), *this)->start();

      start_accept();
    });
  }

  void send_websocket_message(ws_stream_pointer ws, const std::string& message) {
    ws->async_write(net::buffer(message), [&, ws](boost::system::error_code ec, std::size_t) {});
  }

  class Connection : public std::enable_shared_from_this<Connection> {
  private:
    tcp::socket socket;
    Server& server;
    beast::flat_buffer buffer;
    http::request<http::string_body> request;
    ws_stream_pointer ws;

  public:
    Connection(tcp::socket socket, Server& server) 
      : socket(std::move(socket)), server(server) {}

    void start() {
      read_request();
    }

  private:
    void read_request() {
      http::async_read(socket, buffer, request, 
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
          if (!ec)
            self->process_request();
        });
    }

    void process_request() {
      if (websocket::is_upgrade(request)) {
        ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
        ws->async_accept(request, [self = shared_from_this()](boost::system::error_code ec) {
          if (!ec)
            self->process_websocket_request(self->ws, self->request);
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

    void process_websocket_request(ws_stream_pointer ws, const http::request<http::string_body>& req) {
      add_ws_connection(ws);

      auto it = server.ws_endpoints.find(req.target().to_string());
      if (it != server.ws_endpoints.end()) {
        it->second(ws);
      } else {
        ws->async_close(websocket::close_code::normal, [&, ws](boost::system::error_code ec) {
            remove_ws_connection(ws);
        });
      }
    }

    void write_http_response(const http::response<http::string_body>& resp) {
      http::async_write(socket, resp, [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
        if (!ec)
          self->read_request();
      });
    }

    

    void add_ws_connection(ws_stream_pointer ws) {
      server.ws_connections.insert(ws);
    }

    void remove_ws_connection(ws_stream_pointer ws) {
      server.ws_connections.erase(ws);
    }
  };
};