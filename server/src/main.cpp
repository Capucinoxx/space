#include "handler.h"
#include "game_manager.h"
#include "player.h"

#include <atomic>

enum { ROWS = 300, COLS = 300 };

auto game = std::make_shared<GameManager<ROWS, COLS>>();

// class ClientHandler : public WebsocketHandler {
// public:
//   void on_open(Server::ws_stream_pointer ws, Server::http_request req) override {
//     std::cout << "New connection" << std::endl;
//     std::cout << "Token: " << req[http::field::authorization] << std::endl;

//     std::string token = req[http::field::authorization].to_string();
//     game->register_player(std::make_shared<Player<ROWS, COLS>>(token));
//   }

//   void on_message(const std::string& message) override {
//     Player<ROWS, COLS>::direction dir = Player<ROWS, COLS>::parse_action(message);
//     std::cout << "Direction : " << dir << std::endl;
//   }
// };

// todo :
// - plugger WebsocketHandler au bonne place dans le network
// - voir comment mettre la logique du on_message

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
