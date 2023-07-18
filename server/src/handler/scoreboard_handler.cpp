#include "handler/scoreboard_handler.h"
#include <iostream>

void scoreboard_handler::operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) {
  auto [status, body] = handle();
  resp.result(status);
  resp.body() = std::move(body);
  resp.keep_alive(req.keep_alive());
  resp.set(http::field::server, "Space");
  resp.set(http::field::content_type, "application/octet-stream");
  resp.prepare_payload();
}

std::pair<http::status, std::string> scoreboard_handler::handle() {
  std::string body = "ok for now";
  return {http::status::ok, std::move(body)};
}