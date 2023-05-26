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

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class http_server {
public:
  using tcp = net::ip::tcp;
  using http_handler = std::function<void(http::request<http::string_body>&, http::response<http::string_body>&)>;

  using shared_ws_type = std::shared_ptr<websocket::stream<tcp::socket>>;
  using ws_handler = std::function<void(shared_ws_type)>;

private:
  net::io_context ioc;
  tcp::acceptor acceptor;
  std::map<std::string, http_handler> endpoints;
  std::map<std::string, ws_handler> ws_routes;
  std::unordered_set<std::shared_ptr<websocket::stream<tcp::socket>>> ws_connections;

public:
  http_server(short unsigned int port) : acceptor(ioc, { tcp::v4(), port }) {}

  void run() {
    while(true) {
      tcp::socket socket(ioc);
      acceptor.accept(socket);

      auto req = read_http_req(socket);

      if (websocket::is_upgrade(req)) {
        std::shared_ptr<websocket::stream<tcp::socket>> ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(socket));
        ws->accept(req);

        handle_websocket(ws, req.base().target().to_string());
      } else {
        http::response<http::string_body> response;
        handle_http_req(req, response);

        write_http_response(socket, response);
      }
    }
  }

  void add_endpoint(const std::string& path, http_handler handler) {
    endpoints[path] = handler;
  }

  void add_ws_route(const std::string& path, ws_handler handler) {
    ws_routes[path] = handler;
  }

  void broadcast_websocket_message(const std::string& msg) {
    for (auto& ws : ws_connections)
      send_websocket_message(ws, msg);
  }

private:
  http::request<http::string_body> read_http_req(tcp::socket& socket) {
    beast::flat_buffer  buffer;
    http::request<http::string_body> req;
    http::read(socket, buffer, req);
    return req;
  }

  void write_http_response(tcp::socket& socket, const http::response<http::string_body>& resp) {
    http::write(socket, resp);
  }

  void handle_http_req(http::request<http::string_body>& req, http::response<http::string_body>& resp) {
    auto it = endpoints.find(req.target().to_string());
    if (it != endpoints.end())
      it->second(req, resp);
    else {
      resp.result(http::status::not_found);
      resp.set(http::field::content_type, "text/plain");
      resp.body() = "404 Not Found";
    }
  }

  void handle_websocket(std::shared_ptr<websocket::stream<tcp::socket>> ws, const std::string& path) {
    add_ws_connection(ws);

    auto it = ws_routes.find(path);
    if (it != ws_routes.end())
      it->second(ws);
    else
      ws->async_close(websocket::close_code::normal, [&, ws](boost::system::error_code ec) {
        remove_ws_connection(ws);
      });
  }

  void send_websocket_message(std::shared_ptr<websocket::stream<tcp::socket>> ws, const std::string& message) {
    boost::system::error_code ec;
    ws->write(net::buffer(message), ec);
  }

  void add_ws_connection(std::shared_ptr<websocket::stream<tcp::socket>> ws) {
    ws_connections.insert(ws);
  }

  void remove_ws_connection(std::shared_ptr<websocket::stream<tcp::socket>> ws) {
    ws_connections.erase(ws);
  }
};

#endif //SPACE_NETWORK_H
