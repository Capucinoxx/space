#ifndef SPACE_NETWORK_SHARED_STATE_H
#define SPACE_NETWORK_SHARED_STATE_H

#include "network/utils.h"
#include "handler/handler.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <iostream>


class websocket_session;

class shared_state {
public:
	enum protocol_t { HTTP, WEBSOCKET };

private:
	std::string doc_root_;
	std::unordered_map<std::string, std::unordered_set<websocket_session*>> sessions_;

	std::unordered_map<std::string, handler_sptr> http_handlers;
	std::unordered_map<std::string, handler_sptr> channels;

public:
	explicit shared_state(std::string doc_root);

	void join(websocket_session& session, std::string channel);
	void leave(websocket_session& session);

	void send(std::string channel, std::string message);

	std::string const& doc_root() const noexcept { return doc_root_; }

	void add_http_handler(std::string path, handler_sptr handler) {
		http_handlers[path] = handler;
	}

	handler_sptr get_handler(std::string path, protocol_t proto) {
		if (proto == HTTP)
			return http_handlers[path];
		
		std::cout << "get_handler: " << path << std::endl;
		return channels[path];
	}


	void add_channel(std::string path, handler_sptr handler) {
		channels[path] = handler;
	}

	http::response<http::string_body> handle_http_request(http::request<http::string_body>& req);

	bool is_channel(std::string path) {
		return channels.find(path) != channels.end();
	}
};



#endif //SPACE_NETWORK_SHARED_STATE_H