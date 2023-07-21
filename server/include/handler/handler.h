#ifndef SPACE_HANDLER_HANDLER_H
#define SPACE_HANDLER_HANDLER_H

#include "network/utils.h"

#include <memory>
#include <iostream>

class handler {
public:
	virtual void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) = 0;

	virtual bool on_open(http::request<http::string_body>& req) {
		return true;
	}
	virtual void on_close() {}

	virtual void on_message(std::string message) {}
};

using handler_sptr = std::shared_ptr<handler>;

#endif //SPACE_HANDLER_HANDLER_H