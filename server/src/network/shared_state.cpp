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

http::response<http::string_body> shared_state::handle_http_request(http::request<http::string_body>& req) {
  http::response<http::string_body> resp{http::status::not_found, req.version()};
  auto it = http_handlers.find(req.target().to_string());
  if (it != http_handlers.end()) {
    auto ptr = it->second.get();
    (*ptr)(req, resp);
  }
  return resp;
}
