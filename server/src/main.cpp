#include <iostream>
#include <vector>

#include "network.h"
#include "game_manager.h"


enum { ROWS = 300, COLS = 400 };
enum { PORT = 8080 };
enum { TICK = 500 };


int main() {
  Server<ROWS, COLS> server("0.0.0.0", PORT, 10);

  GameManager<ROWS, COLS> game(TICK, [&](const std::vector<uint8_t>& data) {
    server.broadcast(data);
  });

  game.start();


  server.listen();



  return 0;
}