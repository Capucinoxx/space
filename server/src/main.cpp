#include <iostream>

#include "network.h"
#include "map.h"

namespace net = boost::asio;

enum { PORT = 8080 };

int main() {
  using tcp = websocket_server::tcp;

  net::io_context ioc;

  websocket_server server(ioc, tcp::endpoint(tcp::v4(), 8080));

  server.start();
  ioc.run();

  return 0;
}