// #include "network.h"
#include "handler/scoreboard_handler.h"
#include "handler/spectator_handler.h"
#include "handler/controller_handler.h"
#include "handler/game_handler.h"
#include "handler/subscription_handler.h"

#include "middleware/admin_middleware.h"

#include "configuration.h"
#include "postgres_connector.h"
#include "game_loop.h"
#include "game_state.h"

#include "network/utils.h"
#include "network/listener.h"
#include "network/shared_state.h"

#include "player/player.h"
#include "common.h"

#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <string>
#include <atomic>
#include <memory>



game_loop_sptr create_game(const std::string& base_path, std::shared_ptr<shared_state> state, std::shared_ptr<PostgresConnector> postgres, bool with_score_insertion) {
  auto game_state = std::make_shared<GameState<std::pair<player_sptr, std::string>, rows, cols>>(postgres, with_score_insertion);
  auto game_loop = std::make_shared<GameLoop<std::pair<player_sptr, std::string>, tick, max_tick, rows, cols>>(game_state, base_path.substr(1));

  auto gh = std::make_shared<game_handler>(game_state, postgres);
  state->add_channel(base_path + "/game", [game_state, postgres]() { return std::make_shared<game_handler>(game_state, postgres); });
  state->add_channel(base_path + "/spectate", [](){ return std::make_shared<spectator_handler>(); });


  return game_loop;
}

int main() {
  Config cfg = load_config(".env");

  auto is_admin = std::make_shared<admin_middleware>(cfg["ADMIN_PASSWORD"]);

  auto postgres = PostgresConnector::get_instance(cfg);
  if (!postgres->connected()) {
    std::cerr << "Failed to connect to database" << std::endl;
    return 1;
  }

  auto address = net::ip::make_address("0.0.0.0");
  unsigned short port = 8080;
  auto doc_root = "interface";

  net::io_context ioc;

  auto state = std::make_shared<shared_state>(doc_root);


  auto h_scoreboard = std::make_shared<scoreboard_handler>(postgres);
  state->add_http_handler("/scoreboard", h_scoreboard);

  auto ranked = create_game("/ranked", state, postgres, true);
  auto unranked = create_game("/unranked", state, postgres, false);

  auto start_controller = std::make_shared<start_game_handler>(state, ranked, unranked);
  start_controller->add_middleware(is_admin);
  state->add_http_handler("/start_game", start_controller);

  auto stop_controller = std::make_shared<stop_game_handler>(ranked, unranked);
  stop_controller->add_middleware(is_admin);
  state->add_http_handler("/stop_game", stop_controller);

  state->add_http_handler("/subscribe", std::make_shared<subscription_handler>(postgres));

  std::make_shared<listener>(ioc, tcp::endpoint{ address, port }, state)->run();

  net::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait([&](error_code const&, int) { ioc.stop(); });

  ioc.run();

  return 0;
}
