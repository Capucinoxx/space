#include "handler.h"
#include "game_manager.h"
#include "player.h"

#include <atomic>

enum { ROWS = 300, COLS = 300 };

auto game = std::make_shared<GameManager<ROWS, COLS>>();

int main() {  
  Server server(8030);

  server.add_ws_endpoint("/game", [&]() -> std::shared_ptr<WebsocketHandler> {
    return std::make_shared<GameHandler<ROWS, COLS>>(game);
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
