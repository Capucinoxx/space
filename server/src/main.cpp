#include "network.h"
#include "configuration.h"
#include "postgres_connector.h"
#include <atomic>

int main() {
  Config cfg = load_config(".env");

  PostgresConnector& postgres = PostgresConnector::get_instance(cfg);
  if (!postgres.connected()) {
    std::cerr << "Failed to connect to database" << std::endl;
    return 1;
  }


  constexpr std::size_t rows = 30;
  constexpr std::size_t cols = 40;

  Server server(8080);

  auto game = std::make_shared<GameManager<rows, cols>>();

  auto game_handler = [&game](){ return std::make_unique<GameHandler<rows, cols>>(game); };
  server.add_ws_endpoint("/game", game_handler);

  auto spectate_handler = [&game](){ return std::make_unique<SpectateHandler>(); };
  server.add_ws_endpoint("/spectate", spectate_handler);

  auto subscription_handler = [&game](Server::http_request req) -> std::pair<http::status, std::string> {
    std::string body = req.body();
    std::string name = retrieve_field(body, "name");
    std::string color = retrieve_field(body, "color");

    std::string message = "name=" + name + " color=" + color;

    return std::make_pair(http::status::not_found, message);
  };
  server.add_http_endpoint("/subscribe", subscription_handler);

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