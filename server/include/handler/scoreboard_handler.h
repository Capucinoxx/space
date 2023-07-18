#ifndef SPACE_HANDLER_SCOREBOARD_HANDLER_H
#define SPACE_HANDLER_SCOREBOARD_HANDLER_H

#include "handler/handler.h"

#include <iostream>

class scoreboard_handler : public handler {
public:
  scoreboard_handler() = default;
  ~scoreboard_handler() {};

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override;

private:
  std::pair<http::status, std::string> handle();
};

#endif //SPACE_HANDLER_SCOREBOARD_HANDLER_H