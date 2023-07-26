#ifndef SPACE_HANDLER_SUBSCRIPTION_HANDLER_H
#define SPACE_HANDLER_SUBSCRIPTION_HANDLER_H

#include "handler/handler.h"
#include "common.h"
#include "postgres_connector.h"

class subscription_handler : public handler {
private:
  psql_sptr postgres;
  UniqueIDGenerator<15> id_generator;

  static constexpr const char* query = R"(
    INSERT INTO player (name, secret, h, s, l) VALUES ($1, $2, $3, $4, $5)
  )";

public:
  subscription_handler(psql_sptr postgres) : postgres(postgres) {};
  ~subscription_handler() {};

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override;

private:
  std::pair<http::status, std::string> handle(http::request<http::string_body>& req);
};

#endif //SPACE_HANDLER_SUBSCRIPTION_HANDLER_H