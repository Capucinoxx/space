#include "network.h"
#include <atomic>

int main() {
  constexpr std::size_t rows = 20;
  constexpr std::size_t cols = 20;
  auto game = std::make_shared<GameManager<rows, cols>>();

  Server server(8080);

  // Add game handler
  auto game_handler = std::make_shared<GameHandler<rows, cols>>(game);
  server.add_websocket_handler(game_handler);

  // Add spectate handler
  auto spectate_handler = std::make_shared<SpectateHandler>();
  server.add_websocket_handler(spectate_handler);


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