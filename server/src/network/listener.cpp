#include "network/listener.h"
#include "network/session.h"

#include <iostream>

listener::listener(net::io_context& ioc, tcp::endpoint endpoint, std::shared_ptr<shared_state> const& state)
  : acceptor(ioc), socket(ioc), state(state) {
    error_code ec;

    acceptor.open(endpoint.protocol(), ec);
    if (ec) {
      fail(ec, "open");
      return;
    }

    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
      fail(ec, "set_option");
      return;
    }

    acceptor.bind(endpoint, ec);
    if (ec) {
      fail(ec, "bind");
      return;
    }

    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
      fail(ec, "listen");
      return;
    }    
}

void listener::run() {
  acceptor.async_accept(socket, [self = shared_from_this()](error_code ec) {
    self->on_accept(ec);
  });
}

void listener::fail(error_code ec, char const* what) {
  if (ec == net::error::operation_aborted)
    return;

  std::cerr << what << ": " << ec.message() << "\n";
}

void listener::on_accept(error_code ec) {
  if (ec)
    return fail(ec, "accept");

  std::make_shared<http_session>(std::move(socket), state)->run();

  acceptor.async_accept(socket, [self = shared_from_this()](error_code ec) {
    self->on_accept(ec);
  });
}