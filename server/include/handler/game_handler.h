#ifndef SPACE_HANDLER_GAME_HANDLER_H
#define SPACE_HANDLER_GAME_HANDLER_H

#include "handler/handler.h"
#include "postgres_connector.h"
#include "game_state.h"
#include "common.h"

class game_handler : public handler {
private:
  psql_sptr postgres;

  static constexpr const char* query = R"(
    SELECT p.id, p.name, p.h, p.s, p.l, ps.score
    FROM player p
    LEFT JOIN player_scores ps ON ps.player_id = p.id
    WHERE p.secret = '{}'
    ORDER BY ps.id DESC
    LIMIT 1
  )";

public:

  template<uint32_t ROWS, uint32_t COLS>
  game_handler(psql_sptr postgres) : postgres(postgres) {};
  ~game_handler() {};

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override;

  bool on_open(http::request<http::string_body>& req);
  // void on_close() override;

private:
  std::pair<http::status, std::string> handle();
};

#endif //SPACE_HANDLER_GAME_HANDLER_H