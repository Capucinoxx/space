#ifndef SPACE_HANDLER_GAME_HANDLER_H
#define SPACE_HANDLER_GAME_HANDLER_H

#include "handler/handler.h"
#include "postgres_connector.h"
#include "game_state.h"
#include "common.h"
#include "player/player.h"

#include <iostream>

class game_handler : public handler {
private:
  game_sptr game_state;
  psql_sptr postgres;
  player_sptr player;

  static constexpr const char* query = R"(
    SELECT p.id, p.name, p.h, p.s, p.l, ps.score
    FROM player p
    LEFT JOIN player_scores ps ON ps.player_id = p.id
    WHERE p.secret = '%1%'
    ORDER BY ps.id DESC
    LIMIT 1
  )";

public:

  game_handler(game_sptr game_state, psql_sptr postgres) 
    : game_state(game_state), postgres(postgres) {
      std::cout << "CREATE" << std::endl;
    };
  ~game_handler() {};

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override;

  bool on_open(http::request<http::string_body>& req) override;
  void on_close() override;
  void on_message(std::string message) override;

};

#endif //SPACE_HANDLER_GAME_HANDLER_H