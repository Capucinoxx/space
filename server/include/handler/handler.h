#ifndef SPACE_HANDLER_HANDLER_H
#define SPACE_HANDLER_HANDLER_H

#include "network/utils.h"
#include "middleware/middleware.h"

#include <memory>
#include <iostream>
#include <vector>

class handler {
private:
	std::vector<middleware_sptr> middlewares;

public:
	virtual void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) = 0;

	virtual bool on_open(http::request<http::string_body>& req) {
		return true;
	}
	virtual void on_close() {}

	virtual void on_message(std::string message) {}

	void add_middleware(middleware_sptr middleware) {
		middlewares.push_back(middleware);
	}

	bool check_middleware(const http::request<http::string_body>& req) {
		for (auto& middleware : middlewares) {
			if (!(*middleware)(req)) {
				return false;
			}
		}
		return true;
	}
};

using handler_sptr = std::shared_ptr<handler>;

#endif //SPACE_HANDLER_HANDLER_H