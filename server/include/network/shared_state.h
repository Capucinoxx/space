#ifndef SPACE_NETWORK_SHARED_STATE_H
#define SPACE_NETWORK_SHARED_STATE_H

#include <memory>
#include <string>
#include <unordered_set>

class websocket_session;

class shared_state {
private:
	std::string doc_root_;
	std::unordered_set<websocket_session*> sessions_;

public:
	explicit shared_state(std::string doc_root);

	void join(websocket_session& session);
	void leave(websocket_session& session);

	void send(std::string message);

	std::string const& doc_root() const noexcept { return doc_root_; }
};

#endif //SPACE_NETWORK_SHARED_STATE_H