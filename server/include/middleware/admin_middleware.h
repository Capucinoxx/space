#ifndef SPACE_MIDDLEWARE_AUTH_MIDDLEWARE_H
#define SPACE_MIDDLEWARE_AUTH_MIDDLEWARE_H

#include "middleware/middleware.h"
#include "network/utils.h"

#include <string>


class admin_middleware : public middleware {
public:
  std::string admin_password;

  explicit admin_middleware(std::string admin_password) 
    : admin_password(std::move(admin_password)) {}

  bool operator()(const http::request<http::string_body>& req) override {
    std::string password = req[http::field::authorization].to_string();
    return password == admin_password;
  }
};

#endif //SPACE_MIDDLEWARE_AUTH_MIDDLEWARE_H