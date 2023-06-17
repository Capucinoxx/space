#ifndef SPACE_HANDLERS_GAME_HANDLER_H
#define SPACE_HANDLERS_GAME_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include "player.h"

template<uint32_t ROWS, uint32_t COLS>
class GameHandler : public WebSocketHandler{
public:
  using game_ptr = std::shared_ptr<GameManager<ROWS, COLS>>;
  using player_ptr = std::shared_ptr<Player<ROWS, COLS>>;
  using psql_ref = PostgresConnector&;

private:
  game_ptr game;
  psql_ref postgres;

public:
  explicit GameHandler(game_ptr game, psql_ref postgres) 
    : game(game), postgres(postgres) {}

  bool on_open(ws_stream_ptr ws, ws_stream_ptr req) override {
    std::string token = req[http::field::authorization].to_string();
    if (token == "")
      return false;

    std::string query = "SELECT id, name FROM users WHERE token = '" + token + "'";
    auto result = postgres.execute(query);

    if (result.size() == 0)
      return false;

    uint32_t id = std::get<uint32_t>(result[0][0]);
    std::string name = std::get<std::string>(result[0][1]);

    player_ptr player = std::make_shared<Player<ROWS, COLS>>(
      name, id, game_manager->frame(), game_manager->get_grid());
    game_manager->register_player(player);

    return true;
  }

  void on_message(const std::string& message) override {
    if (!game_manager->is_running())
      return;
    
    typename Player<ROWS, COLS>::direction direction = Player<ROWS, COLS>::parse_action(message);
    auto res = player->perform(game_manager->frame(), direction);
    
    game_manager->handle_move_result(player, res);
  }

  bool handle_message() override { return true; }
};

#endif //SPACE_HANDLERS_GAME_HANDLER_H