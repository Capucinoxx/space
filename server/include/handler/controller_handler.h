#ifndef SPACE_HANDLER_CONTROLLER_HANDLER_H
#define SPACE_HANDLER_CONTROLLER_HANDLER_H

#include "handler/handler.h"
#include "game_loop.h"
#include "network/shared_state.h"
#include "common.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

class start_game_handler : public handler {
private:
    shared_state_sptr state;
    std::vector<game_loop_sptr> game_loops;

public:
    template<typename... Args>
    start_game_handler(shared_state_sptr state, Args&&... args)
        : state(state), game_loops{ std::forward<Args>(args)... } {}

    void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override {
        if (!check_middleware(req)) {
            resp.result(http::status::unauthorized);
            return;
        }

        std::for_each(game_loops.begin(), game_loops.end(), [this](auto& game_loop) {
            game_loop->start(state.get(), &shared_state::send);
        });

        resp.result(http::status::ok);
    }
};



class stop_game_handler : public handler {
private:
  std::vector<game_loop_sptr> game_loops;

public:
  template<typename... Args>
  explicit stop_game_handler(Args&&... args) : game_loops{ std::forward<Args>(args)... } {}

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override {
    if (!check_middleware(req)) {
      resp.result(http::status::unauthorized);
      return;
    }

    std::for_each(game_loops.begin(), game_loops.end(), [](auto& game_loop) {
      game_loop->stop();
    });

    resp.result(http::status::ok);
  }
};

class increase_multiplier_handler : public handler {
private:
  game_sptr game;

public:
  explicit increase_multiplier_handler(game_sptr game) : game{ game } {}

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override {
    if (!check_middleware(req)) {
      resp.result(http::status::unauthorized);
      return;
    }

    auto current_multiplier = game->increase_multiplier();
    resp.body() = std::to_string(current_multiplier);
    resp.result(http::status::ok);
  }
};

class decrease_multiplier_handler : public handler {
private:
  game_sptr game;

public:
  explicit decrease_multiplier_handler(game_sptr game) : game{ game } {}

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override {
    if (!check_middleware(req)) {
      resp.result(http::status::unauthorized);
      return;
    }

    auto current_multiplier = game->decrease_multiplier();
    resp.body() = std::to_string(current_multiplier);
    resp.result(http::status::ok);
  }
};

#endif //SPACE_HANDLER_CONTROLLER_HANDLER_H