// #include "network.h"
#include "handler/scoreboard_handler.h"
#include "handler/spectator_handler.h"
#include "handler/game_handler.h"
#include "configuration.h"
#include "postgres_connector.h"
#include "game_loop.h"
#include "game_state.h"

// #include "handlers/subscription_hander.h"
// #include "handlers/game_handler.h"

#include <atomic>
#include <memory>

#include "network/utils.h"
#include "network/listener.h"
#include "network/shared_state.h"
#include "player/player.h"

#include "common.h"


#include <boost/asio/signal_set.hpp>
#include <iostream>

// constexpr std::size_t rows = 100;
// constexpr std::size_t cols = 100;
// constexpr std::size_t tick = 100;
// constexpr std::size_t max_tick = 1000;

// using player_ptr = std::shared_ptr<Player<rows, cols>>;
// using action_t = std::pair<player_ptr, std::string>;

// using game_loop_ptr = std::shared_ptr<GameLoop<std::pair, tick, max_tick, rows, cols>>;

game_loop_sptr create_game(const std::string& base_path, std::shared_ptr<shared_state> state, std::shared_ptr<PostgresConnector> postgres, bool with_score_insertion) {
  auto game_state = std::make_shared<GameState<std::pair<player_sptr, std::string>, rows, cols>>(postgres, with_score_insertion);
  auto game_loop = std::make_shared<GameLoop<std::pair<player_sptr, std::string>, tick, max_tick, rows, cols>>(game_state);

  state->add_channel(base_path + "/game", std::make_shared<game_handler>(game_state, postgres));
  // GameHandle<rows, cols>(game_state, postgres)(state, base_path);

  // auto spectate_handler = [&game_loop](){ return std::make_unique<SpectateHandler>(); };
  state->add_channel(base_path + "/spectate", std::make_shared<spectator_handler>());


  return game_loop;
}

// game_loop_ptr create_game(const std::string& endpoint, Server& server, std::shared_ptr<PostgresConnector> postgres, bool with_score_insertion) {
//   auto game_state = std::make_shared<GameState<action_t, rows, cols>>(postgres, with_score_insertion);
//   auto game_loop = std::make_shared<GameLoop<action_t, tick, max_tick, rows, cols>>(game_state);

//   GameHandle<rows, cols>(game_state, postgres)(server, endpoint);

//   auto spectate_handler = [&game_loop](){ return std::make_unique<SpectateHandler>(); };
//   server.add_ws_endpoint(endpoint + "/spectate", spectate_handler);

//   return game_loop;
// }


int main() {
    Config cfg = load_config(".env");

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



  std::make_shared<listener>(ioc, tcp::endpoint{ address, port }, state)->run();

  net::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait([&](error_code const&, int) { ioc.stop(); });

  ioc.run();

// 

  // auto admin_middleware = AdminMiddleware(cfg["ADMIN_PASSWORD"]);

  // Server server(8080);

  // auto ranked = create_game("/ranked", server, postgres, true);
  // auto unranked = create_game("/unranked", server, postgres, false);

  // (SubscriptionHandle<action_t, rows, cols>(postgres))(server);
  // (ScoreboardHandle<rows, cols>(postgres))(server);

  // server.add_http_endpoint("/start_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
  //   if (!admin_middleware(req))
  //     return std::make_pair(http::status::unauthorized, "Unauthorized");

  //   ranked->start(&server, &Server::broadcast_websocket_message, "ranked");
  //   unranked->start(&server, &Server::broadcast_websocket_message, "unranked");
  //   return std::make_pair(http::status::ok, "Game started");
  // });

  // server.add_http_endpoint("/stop_game", [&](Server::http_request req) -> std::pair<http::status, std::string> {
  //   if (!admin_middleware(req))
  //     return std::make_pair(http::status::unauthorized, "Unauthorized");

  //   ranked->stop();
  //   unranked->stop();
  //   return std::make_pair(http::status::ok, "Game stopped");
  // }); 

  // server.run();

  // return 0;
}
