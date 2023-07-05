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
#include <shared_mutex>
#include <vector>
#include <algorithm>

#include "game_state.h"
#include "player.h"



std::shared_mutex ws_connections_mu;

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

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

  std::unordered_set<ws_stream_ptr> ws_connections{};
  std::vector<std::thread> thread_pool;

public:
  explicit Server(short unsigned int port, std::size_t thread_pool_size = 4)
    : acceptor(ioc, { tcp::v4(), port }), thread_pool(thread_pool_size) {
      ws_connections.clear();
    };

  ~Server() {
    for (const auto& ws : ws_connections)
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

  void broadcast_websocket_message(const std::vector<uint8_t>& message) {
    std::shared_lock<std::shared_mutex> lock(ws_connections_mu);
    for (auto& ws : ws_connections) {
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
      } else {
        status = http::status::not_found;
        body = "The resource '" + request.target().to_string() + "' was not found.";
      }

      http::response<http::string_body> response(status, request.version());
      prepare_http_response(request, response, body);

      beast::error_code ec;
      http::write(socket, std::move(response), ec);
      socket.shutdown(tcp::socket::shutdown_send, ec);
    }

    void upgrade_websocket() {
      auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
      ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
      ws->async_accept(request, [this, ws, self = shared_from_this()](beast::error_code ec) {
        if (!ec)
          handle_websocket_upgrade(ws);
      });
    }

    void handle_websocket_upgrade(ws_stream_ptr ws) {
      auto it = server.ws_endpoints.find(request.target().to_string());
      if (it != server.ws_endpoints.end()) {
        handler = it->second();
        if (handler->on_open(ws, request)) {
          std::unique_lock<std::shared_mutex> lock(ws_connections_mu);
          server.ws_connections.insert(ws);

          if (handler->handle_message())
            handle_websocket_message(ws);
        }
      } else
        ws->async_close(websocket::close_code::normal, [](beast::error_code ec) {});
    }

    void handle_websocket_message(ws_stream_ptr ws) {
      ws->async_read(buffer, [this, ws, self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (!ec) {
          handler->on_message(beast::buffers_to_string(buffer.data()));
          buffer.consume(buffer.size());
          handle_websocket_message(ws);
        } else {
          std::unique_lock<std::shared_mutex> lock(ws_connections_mu);
          server.ws_connections.erase(ws);
          handler->on_close();
        }
      });
    }
  };
};


class SpectateHandler : public WebSocketHandler {
public:
  bool on_open(ws_stream_ptr ws, http_request req) override { return true; }
  void on_message(const std::string& message) override {}

  void on_close() override {}

  bool handle_message() { return false; }
};



#endif //SPACE_NETWORK_H
