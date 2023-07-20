#include "network/shared_state.h"
#include "network/session.h"

shared_state::shared_state(std::string doc_root)
  : doc_root_(std::move(doc_root)) {}

void shared_state::join(websocket_session& session, std::string channel) {
  sessions_[channel].insert(&session);
}

void shared_state::leave(websocket_session& session) {
  for (auto& [_, sessions] : sessions_)
    sessions.erase(&session);
}

void shared_state::send(const std::string& channel, const std::vector<uint8_t>& message) {
  auto const ss = std::make_shared<std::vector<uint8_t> const>(std::move(message));

  for (auto session : sessions_[channel])
    session->send(ss);
}

http::response<http::string_body> shared_state::handle_http_request(http::request<http::string_body>& req) {
  http::response<http::string_body> resp{http::status::not_found, req.version()};
  auto it = http_handlers.find(req.target().to_string());
  if (it != http_handlers.end()) {
    auto ptr = it->second.get();
    (*ptr)(req, resp);
  }
  return resp;
}
