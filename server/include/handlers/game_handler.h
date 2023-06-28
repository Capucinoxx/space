#ifndef SPACE_HANDLERS_GAME_HANDLER_H
#define SPACE_HANDLERS_GAME_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include "player.h"

template<uint32_t ROWS, uint32_t COLS>
class GameHandler : public WebSocketHandler{
public:
  using player_ptr = std::shared_ptr<Player<ROWS, COLS>>;
  using action_t = std::pair<player_ptr, std::string>;

  using game_ptr = std::shared_ptr<GameState<action_t, ROWS, COLS>>;
  
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

    std::string query = "SELECT id, name, h, s, l FROM player WHERE secret = '" + token + "'";
    auto result = postgres.execute(query);

    if (result.size() != 1)
      return false;

    uint32_t id = result[0][0].as<uint32_t>(0);
    std::string name = result[0][1].as<std::string>("");
    using float64_t = Player<ROWS, COLS>::float64_t;

    float64_t h = result[0][2].as<float64_t>(0.0);
    float64_t s = result[0][3].as<float64_t>(0.0);
    float64_t l = result[0][4].as<float64_t>(0.0);

    player = game->register_player(name, id, { h, s, l });
    return player != nullptr;
  }

  void on_close() {
    if (player != nullptr)
      game->disconnect_player(player->id());
  }

  void on_message(const std::string& message) override {
    game->push({ player, message });
  }

  bool handle_message() override { return true; }
};

template<uint32_t ROWS, uint32_t COLS>
class GameHandle {
public:
  using player_ptr = std::shared_ptr<Player<ROWS, COLS>>;
  using action_t = std::pair<player_ptr, std::string>;

  using game_ptr = std::shared_ptr<GameState<action_t, ROWS, COLS>>;
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