#ifndef SPACE_HANDLER_HANDLER_H
#define SPACE_HANDLER_HANDLER_H

#include "network/utils.h"

#include <memory>

class handler {
public:
	virtual void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) = 0;

	void on_open() {}
	void on_close() {}
};

using handler_sptr = std::shared_ptr<handler>;

#endif //SPACE_HANDLER_HANDLER_H