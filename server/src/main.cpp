#include <iostream>
#include <thread>
#include <chrono>

#include "network.h"
#include "map.h"

enum { PORT = 8080 };
enum { ROWS = 400, COLS = 300 };

int main() {
  map<ROWS, COLS> m;

  http_server server(PORT);

  server.add_ws_route("/game", [](http_server::shared_ws_type ws) {});

  std::thread t([&]() {
    while(true) {
      server.broadcast_websocket_message(m.serialize());
      std::this_thread::sleep_for((std::chrono::milliseconds(1000)));
    }
  });

  server.run();

  t.join();

  return 0;
}