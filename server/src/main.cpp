#include "network.h"
#include "configuration.h"
#include "postgres_connector.h"

#include "handlers/subscription_hander.h"
#include "handlers/game_handler.h"
#include "handlers/scoreboard_handler.h"
#include <atomic>

int main() {
  Config cfg = load_config(".env");

  PostgresConnector& postgres = PostgresConnector::get_instance(cfg);
  if (!postgres.connected()) {
    std::cerr << "Failed to connect to database" << std::endl;
    return 1;
  }

  constexpr std::size_t rows = 200;
  constexpr std::size_t cols = 60;

  Server server(8080);

  auto game = std::make_shared<GameManager<rows, cols>>(postgres);

  SubscriptionHandle<rows, cols>(game, postgres)(server);
  GameHandle<rows, cols>(game, postgres)(server);
  ScoreboardHandle<rows, cols>(game, postgres)(server);

  auto spectate_handler = [&game](){ return std::make_unique<SpectateHandler>(); };
  server.add_ws_endpoint("/spectate", spectate_handler);

  server.add_http_endpoint("/start_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
    game->start(&server, &Server::broadcast_websocket_message);
    return std::make_pair(http::status::ok, "Game started");
  });

  server.add_http_endpoint("/stop_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
    game->stop();
    return std::make_pair(http::status::ok, "Game stopped");
  }); 

  server.run();

  return 0;
}