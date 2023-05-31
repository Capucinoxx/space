#include "network.h"
#include "game_manager.h"

#include <atomic>

enum { ROWS = 300, COLS = 300 };

int main() {
  auto game = std::make_shared<GameManager<ROWS, COLS>>();
  Server server(8030);

  server.add_ws_endpoint("/game", [](Server::ws_stream_pointer ws) {
    std::cout << "New connection" << std::endl;

  //   while (true) {
  //     beast::flat_buffer buffer;
  //     ws->async_read(buffer, [](boost::system::error_code ec, std::size_t bytes_transferred) {
  //       if (!ec) {
  //         // Traitement des données reçues depuis le client WebSocket
  //         std::cout << "received data" << std::endl;
  //       }        else
  //         return;
  //     });
  //   }
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
