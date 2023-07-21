#ifndef SPACE_NETWORK_LISTENER_H
#define SPACE_NETWORK_LISTENER_H

#include "network/utils.h"

#include <memory>
#include <string>

class shared_state;

class listener : public std::enable_shared_from_this<listener> {
private:
  tcp::acceptor acceptor;
  tcp::socket socket;
  std::shared_ptr<shared_state> state;

  void fail(error_code ec, char const* what);
  void on_accept(error_code ec);

public:
  listener(net::io_context& ioc, tcp::endpoint endpoint, std::shared_ptr<shared_state> const& state);

  void run();
};

#endif //SPACE_NETWORK_LISTENER_H