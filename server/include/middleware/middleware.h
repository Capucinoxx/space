#ifndef SPACE_MIDDLEWARE_MIDDLEWARE_H
#define SPACE_MIDDLEWARE_MIDDLEWARE_H

#include "common.h"

#include <memory>

class middleware {
public:
  virtual bool operator()(const http::request<http::string_body>& req) = 0;
};

using middleware_sptr = std::shared_ptr<middleware>;

#endif //SPACE_MIDDLEWARE_MIDDLEWARE_H