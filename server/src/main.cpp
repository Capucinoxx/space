#include "network.h"
#include "configuration.h"
#include "postgres_connector.h"
#include "game_loop.h"
#include "game_state.h"

#include "handlers/subscription_hander.h"
#include "handlers/game_handler.h"
#include "handlers/scoreboard_handler.h"
#include <atomic>

int main() {
  Config cfg = load_config(".env");

  auto postgres = PostgresConnector::get_instance(cfg);

  if (!postgres->connected()) {
    std::cerr << "Failed to connect to database" << std::endl;
    return 1;
  }

  constexpr std::size_t rows = 100;
  constexpr std::size_t cols = 100;
  constexpr std::size_t tick = 100;

  Server server(8080);


  using player_ptr = std::shared_ptr<Player<rows, cols>>;
  using action_t = std::pair<player_ptr, std::string>;

  auto game_state = std::make_shared<GameState<action_t, rows, cols>>(postgres);
  auto game_loop = std::make_shared<GameLoop<action_t, tick, rows, cols>>(game_state);

  SubscriptionHandle<rows, cols>(game_state, postgres)(server);
  GameHandle<rows, cols>(game_state, postgres)(server, "/ranked");
  (ScoreboardHandle<rows, cols>(postgres))(server);

  auto spectate_handler = [&game_loop](){ return std::make_unique<SpectateHandler>(); };
  server.add_ws_endpoint("/spectate", spectate_handler);

  server.add_http_endpoint("/start_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
    game_loop->start(&server, &Server::broadcast_websocket_message);
    return std::make_pair(http::status::ok, "Game started");
  });

  server.add_http_endpoint("/stop_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
    game_loop->stop();
    return std::make_pair(http::status::ok, "Game stopped");
  }); 

  server.run();

  return 0;
}
