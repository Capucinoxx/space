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

class WebsocketHandler;

class Server {
public:
  using tcp = net::ip::tcp;

  using http_request = http::request<http::string_body>&;
  using http_response = http::response<http::string_body>&;
  using http_handler = std::function<void(http_request, http_response)>;

  using ws_stream_pointer = std::shared_ptr<websocket::stream<tcp::socket>>;
  // using ws_handler = std::function<void(ws_stream_pointer, http_request)>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> http_endpoints;
  std::map<std::string, WebsocketHandler> ws_endpoints;

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

  void add_ws_endpoint(const std::string& path, const WebsocketHandler& handler) {
    ws_endpoints[path] = handler;
  }

  void broadcast_websocket_message(const std::vector<uint8_t>& message) {
    for (auto& ws : ws_connections)
      if (ws->is_open())
        ws->async_write(net::buffer(message), [](boost::system::error_code ec, std::size_t) {});
  }

private:
  void start_accept() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
      if (!ec)
        std::make_shared<Connection>(std::move(socket), *this)->start();

      start_accept();
    });
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

    void process_websocket_request(ws_stream_pointer ws, http::request<http::string_body>& req) {
      add_ws_connection(ws);

      auto it = server.ws_endpoints.find(req.target().to_string());
      if (it != server.ws_endpoints.end()) {
        it->second.on_open(ws, req);
        // it->second(ws, req); // TODO: a corrgier

        read_websocket_message();
      } else {
        ws->async_close(websocket::close_code::normal, [&, ws](boost::system::error_code ec) {
          remove_ws_connection();
        });
      }
    }


    void read_websocket_message() {
      ws->async_read(buffer, [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
          std::string message(beast::buffers_to_string(self->buffer.data()));
          self->buffer.consume(bytes_transferred);

          it->second.on_message(message); // todo: a corriger (doit passer iterateur + info player)
          // - on devrait stocker info joueurs quelque part

          std::cout << "received message: " << message << std::endl; // todo a corriger

          self->read_websocket_message();
        } else {
          self->remove_ws_connection();
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
      server.ws_connections.insert(ws);
    }

    void remove_ws_connection() {
      std::lock_guard<std::mutex> lock(server.mu);
      server.ws_connections.erase(ws);
    }
  };
};

class WebsocketHandler {
public:
  virtual void on_open(Server::ws_stream_pointer ws, Server::http_request req) = 0;
  virtual void on_message(const std::string& message) = 0;
};
