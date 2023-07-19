#ifndef SPACE_HANDLER_HANDLER_H
#define SPACE_HANDLER_HANDLER_H

#include "network/utils.h"

#include <memory>

class handler {
public:
	virtual void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) = 0;

	bool on_open(http::request<http::string_body>& req) {
		return true;
	}
	void on_close() {}
	void on_message(std::string message) {}
};

using handler_sptr = std::shared_ptr<handler>;

#endif //SPACE_HANDLER_HANDLER_H