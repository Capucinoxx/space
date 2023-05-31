#include "network.h"
#include "game_manager.h"

#include <atomic>

enum { ROWS = 300, COLS = 300 };

int main() {
  auto game = std::make_shared<GameManager<ROWS, COLS>>();
  Server server(8030);

  server.add_ws_endpoint("/game", [](Server::ws_stream_pointer ws, const Server::http_request req) {
    std::cout << "New connection" << std::endl;
    std::cout << "Token: " << req[http::field::authorization] << std::endl;


    // todo register
    // - aller chercher le token
    // pour le moment juste associer token  == nouveau player, pas de verif
  });


  std::atomic<bool> running{ true };
  auto th = std::thread([&]() {
    while(running) {
      game->update();
      auto data = game->serialize();

      server.broadcast_websocket_message(data);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  server.run();

  return 0;
}
