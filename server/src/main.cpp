#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <chrono>

#include "network.h"
#include "game_manager.h"


enum { ROWS = 300, COLS = 400 };
enum { PORT = 8080 };
enum { TICK = 500 };


int main() {
  auto game = std::make_shared<GameManager<ROWS, COLS>>();
  Server<ROWS, COLS> server("0.0.0.0", PORT, 10, game);


  std::atomic<bool> running{ true };
  auto th = std::thread([&]() {
    while(running) {
      game->update();
      auto data = game->serialize();
      server.broadcast(data);

      std::this_thread::sleep_for(std::chrono::milliseconds(TICK));
    }
  });

  server.listen();

  th.join();


  return 0;
}