#include "handler/subscription_handler.h"

void subscription_handler::operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) {
  auto [status, body] = handle(req);
  resp.result(status);
  resp.body() = std::move(body);
  resp.keep_alive(req.keep_alive());
  resp.set(http::field::server, "Space");
  resp.set(http::field::content_type, "application/octet-stream");
  resp.set(http::field::access_control_allow_origin, "*");
  resp.prepare_payload();
}

std::pair<http::status, std::string> subscription_handler::handle(http::request<http::string_body>& req) {
  std::string body = req.body();
  std::string name = retrieve_field(body, "name");
  std::string color = retrieve_field(body, "color");

  if (name.empty() || color.empty())
    return {http::status::bad_request, "Missing name or color"};

  if (count_unicode_chars(name) > 16)
    return {http::status::bad_request, "Name too long"};

  auto hsl = parse_color(color);
  if (!hsl)
    return {http::status::bad_request, "Invalid color"};

  auto secret = id_generator();

  auto result = postgres->execute(query, name, secret, std::get<0>(*hsl), std::get<1>(*hsl), std::get<2>(*hsl));

  return result.affected_rows() == 1 
    ? std::make_pair(http::status::ok, secret) 
    : std::make_pair(http::status::bad_request, "Name already taken");
}