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
#include <algorithm>

#include "game_manager.h"
#include "player.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class WebSocketHandler {
public:
  using tcp = net::ip::tcp;
  using ws_stream_ptr = std::shared_ptr<websocket::stream<tcp::socket>>;
  using http_request = http::request<http::string_body>&;

  virtual void on_open(ws_stream_ptr ws, http_request req) = 0;
  virtual void on_message(const std::string& message) = 0;

  virtual bool handle_message() = 0;
};

class Server {
public:
  using tcp = net::ip::tcp;

  using http_request = http::request<http::string_body>&;
  using http_response = http::response<http::string_body>&;
  using http_handler = std::function<void(http_request, http_response)>;

  using ws_stream_ptr = std::shared_ptr<websocket::stream<tcp::socket>>;
  using ws_handler = std::function<std::unique_ptr<WebSocketHandler>()>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> http_endpoints;
  std::map<std::string, ws_handler>   ws_endpoints;

  std::unordered_set<ws_stream_ptr> ws_connections;
  std::vector<std::thread> thread_pool;
  std::mutex mu;

public:
  explicit Server(short unsigned int port, std::size_t thread_pool_size = 4)
    : acceptor(ioc, { tcp::v4(), port }), thread_pool(thread_pool_size) { };

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
        handler->on_open(ws, request);
        server.ws_connections.insert(ws);

        if (handler->handle_message())
          handle_websocket_message(ws);
      }
    }

    void handle_websocket_message(ws_stream_ptr ws) {
      ws->async_read(buffer, [this, ws](beast::error_code ec, std::size_t) {
        if (!ec) {
          handler->on_message(beast::buffers_to_string(buffer.data()));
          buffer.consume(buffer.size());
          handle_websocket_message(ws);
        }
      });
    }
  };
};

template<std::size_t ROWS, std::size_t COLS>
class GameHandler : public WebSocketHandler {
private:
  std::shared_ptr<Player<ROWS, COLS>> player;
  std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

public:
  explicit GameHandler(std::shared_ptr<GameManager<ROWS, COLS>> game_manager)
    : game_manager(game_manager) { };

  void on_open(ws_stream_ptr ws, http_request req) override {
    std::string token = req[http::field::cookie].to_string();
    player = std::make_shared<Player<ROWS, COLS>>(token, game_manager->frame(), game_manager->get_grid());

    game_manager->register_player(player);
  };

  void on_message(const std::string& message) override {
    typename Player<ROWS, COLS>::direction direction = Player<ROWS, COLS>::parse_action(message);
    player->perform(game_manager->frame(), direction);
  }

  bool handle_message() { return true; }
}; 

class SpectateHandler : public WebSocketHandler {
public:
  void on_open(ws_stream_ptr ws, http_request req) override {}
  void on_message(const std::string& message) override {}

  bool handle_message() { return false; }
};

#endif //SPACE_NETWORK_H
