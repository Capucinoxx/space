#include "network.h"

int main() {
  Server server(8080);

  server.add_ws_endpoint("/game", [](Server::ws_stream_pointer ws) {
    std::cout << "New connection" << std::endl;
  });

  server.run();

  return 0;
}
