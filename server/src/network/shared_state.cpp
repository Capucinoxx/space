#include "network/shared_state.h"
#include "network/session.h"

shared_state::shared_state(std::string doc_root)
  : doc_root_(std::move(doc_root)) {}

void shared_state::join(websocket_session& session) {
  sessions_.insert(&session);
}

void shared_state::leave(websocket_session& session) {
  sessions_.erase(&session);
}

void shared_state::send(std::string message) {
  auto const ss = std::make_shared<std::string const>(std::move(message));

  for (auto session : sessions_)
    session->send(ss);
}

std::pair<http::status, std::string> shared_state::handle_http_request(http::request<http::string_body>& req) {
  auto it = http_handlers.find(req.target().to_string());
  if (it != http_handlers.end())
    return it->second(req);
  return {http::status::not_found, "Not found"};
}
