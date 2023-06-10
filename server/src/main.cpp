#include "network.h"
#include <atomic>

int main() {
  constexpr std::size_t rows = 30;
  constexpr std::size_t cols = 40;
  auto game = std::make_shared<GameManager<rows, cols>>();

  Server server(8080);

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