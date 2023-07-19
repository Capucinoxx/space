#ifndef SPACE_HANDLER_SPECTATOR_HANDLER_H
#define SPACE_HANDLER_SPECTATOR_HANDLER_H

#include "handler/handler.h"
#include "game_state.h"
#include "common.h"

class spectator_handler : public handler {  
public:
  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override {}
};

#endif //SPACE_HANDLER_SPECTATOR_HANDLER_H