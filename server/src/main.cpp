#include <iostream>
#include <thread>
#include <chrono>

#include "game_manager.h"
#include "network.h"

enum { PORT = 8080 };
enum { ROWS = 400, COLS = 300 };

int main() {
  using Server = HttpServer<ROWS, COLS>;


  Server server(PORT);

  std::function<void(const std::vector<uint8_t>&)> broadcastFunc = [&server](const std::vector<uint8_t>& message) {
    server.broadcast_message(message);
  };

  GameManager<ROWS, COLS> game(150, broadcastFunc);

  server.add_ws_route("/game", [&](Server::http_request req, Server::ws_session ws) {

   });

   game.start();

   server.run();

  return 0;
}