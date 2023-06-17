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
  player_ptr player;

public:
  explicit GameHandler(game_ptr game, psql_ref postgres) 
    : game(game), postgres(postgres) {}

  bool on_open(WebSocketHandler::ws_stream_ptr ws, WebSocketHandler::http_request req) override {
    std::string token = req[http::field::authorization].to_string();
    if (token == "")
      return false;

    std::string query = "SELECT id, name FROM player WHERE secret = '" + token + "'";
    auto result = postgres.execute(query);

    if (result.size() != 1)
      return false;

    uint32_t id = result[0][0].as<uint32_t>(0);
    std::string name = result[0][1].as<std::string>("");

    player_ptr player = std::make_shared<Player<ROWS, COLS>>(
      name, id, game->frame(), game->get_grid());
    game->register_player(player);

    return true;
  }

  void on_message(const std::string& message) override {
    if (!game->is_running())
      return;
    
    typename Player<ROWS, COLS>::direction direction = Player<ROWS, COLS>::parse_action(message);
    auto res = player->perform(game->frame(), direction);
    
    game->handle_move_result(player, res);
  }

  bool handle_message() override { return true; }
};

template<uint32_t ROWS, uint32_t COLS>
class GameHandle {
public:
  using game_ptr = std::shared_ptr<GameManager<ROWS, COLS>>;
  using psql_ref = PostgresConnector&;

private:
  game_ptr game;
  psql_ref postgres;

public:
  GameHandle(game_ptr game, psql_ref postgres) : game(game), postgres(postgres) {}

  void operator()(Server& server) {
    server.add_ws_endpoint("/game", [&](){ 
      return std::make_unique<GameHandler<ROWS, COLS>>(game, postgres); 
    });
  }
};

#endif //SPACE_HANDLERS_GAME_HANDLER_H