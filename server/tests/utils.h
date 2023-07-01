#ifndef SPACE_TESTS_UTILS_H
#define SPACE_TESTS_UTILS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

#include "game_state.h"
#include "player.h"
#include "tests.h"

enum { ROWS = 300, COLS = 400 }; 

enum direction {
  UP = '\0',
  DOWN = '\x01',
  LEFT = '\x02',
  RIGHT = '\x03'
};

using position = std::pair<uint32_t, uint32_t>;

struct Bot {
  uint32_t uuid; // < unique id
  position spawn_pos;
};

using action = std::pair<uint32_t, direction>;
using actions = std::vector<action>;

struct expectation {
  uint32_t uuid;
  position pos;

  uint32_t tick_alive;
  // std::vector<double> tick_scores;

  // std::vector<position> trail;
  // std::vector<position> region;
};

using Expectations = std::vector<expectation>;

using Bots = std::vector<Bot>;
using Ticks = std::vector<actions>;

struct Scenario {
  Bots bots;
  Ticks ticks;
  Expectations expected_positions;

  void run(const std::string& name) {
    assert::it(name, [&](){
      // ----------------- sanity check
      assert::is_false(bots.empty());
      std::unordered_map<uint32_t, std::shared_ptr<Player<ROWS, COLS>>> players{};

      // ----------------- setup game
      GameState<std::pair<std::shared_ptr<Player<ROWS, COLS>>, std::string>, ROWS, COLS> game(nullptr);

      for (uint32_t i = 0; i != bots.size(); ++i) {
        auto& bot = bots[i];
        auto player = game.register_player("", bot.uuid, { 0.0, 0.0, 0.0 }, 0.0, bot.spawn_pos);

        players.insert({ bot.uuid, player });      
        assert::equal(player->pos(), bot.spawn_pos);
      }

      // ----------------- run game
      for (const auto& tick : scenario.ticks) {
        for (const auto& [uuid, dir] : tick) {
          game.push({ players[uuid], std::to_string(dir) });
        }

        game.play_tick();
      }

      // ----------------- check results
      assert_scenario_result(game);
      assert_grid_and_player_has_same_context(game);
    });
  }

private:
  void assert_scenario_result(GameState<std::pair<std::shared_ptr<Player<ROWS, COLS>>, std::string>, ROWS, COLS>& game) {
    for (const auto& expectation : scenario.expected_positions) {
      auto& [uuid, pos, tick_alive] = expectation;
      assert::equal(players[uuid]->pos(), pos);
      assert::equal(players[uuid]->tick_alive(), tick_alive);
    }
  }

  void assert_grid_and_player_has_same_context(GameState<std::pair<std::shared_ptr<Player<ROWS, COLS>>, std::string>, ROWS, COLS>& game) {
    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> grid_region_context{};
    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> grid_trail_context{};

    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> player_region_context{};
    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> player_trail_context{};

    for (uint32_t row = 0; row != ROWS; ++row) {
      for (uint32_t col = 0; col != COLS; ++col) {
        auto owner = game.cell({row, col}).owner();
        if (owner == 0)
          continue;

        if (game.cell({row, col}).is_step()) {
          grid_trail_context[owner].push_back({row, col});
        } else {
          grid_region_context[owner].push_back({row, col});
        }
      }
    }

    for (const auto& [uuid, player] : players) {
      player->for_each_trail([&](const auto& pos) {
        player_trail_context[uuid].push_back(pos);
      });

      player->for_each_region([&](const auto& pos) {
        player_region_context[uuid].push_back(pos);
      });
    }


    for (const auto& [uuid, grid_context] : grid_region_context) {
      assert::is_true(player_region_context.find(uuid) != player_region_context.end(), std::to_string(uuid) + " not found in player_region_context");
      if (player_region_context.find(uuid) != player_region_context.end())
        assert::equal_unordered(grid_context, player_region_context[uuid], "grid_region_context and player_region_context are not equal");

      player_region_context.erase(uuid);
    }

    for (const auto& [uuid, grid_context] : grid_trail_context) {
      assert::is_true(player_trail_context.find(uuid) != player_trail_context.end(), std::to_string(uuid) + " not found in player_trail_context");
      if (player_trail_context.find(uuid) != player_trail_context.end())
        assert::equal_unordered(grid_context, player_trail_context[uuid], "grid_trail_context and player_trail_context are not equal");

      player_trail_context.erase(uuid);
    }

    assert::is_true(player_region_context.empty(), "player_region_context is not empty");
    assert::is_true(player_trail_context.empty(), "player_trail_context is not empty");
  }
};


#endif // SPACE_TESTS_UTILS_H
