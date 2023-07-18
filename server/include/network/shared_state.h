#ifndef SPACE_NETWORK_SHARED_STATE_H
#define SPACE_NETWORK_SHARED_STATE_H

#include "network/utils.h"
#include "handler/handler.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>


class websocket_session;

class shared_state {
private:
	std::string doc_root_;
	std::unordered_set<websocket_session*> sessions_;

	std::unordered_map<std::string, handler_sptr> http_handlers;
	std::unordered_map<std::string, handler_sptr> channels;

public:
	explicit shared_state(std::string doc_root);

	void join(websocket_session& session);
	void leave(websocket_session& session);

	void send(std::string message);

	std::string const& doc_root() const noexcept { return doc_root_; }

	void add_http_handler(std::string path, handler_sptr handler) {
		http_handlers[path] = handler;
	}

	http::response<http::string_body> handle_http_request(http::request<http::string_body>& req);
};



#endif //SPACE_NETWORK_SHARED_STATE_H