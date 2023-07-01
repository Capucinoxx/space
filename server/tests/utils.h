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
};
// using expectation = std::pair<uint32_t, position>;
using Expectations = std::vector<expectation>;

using Bots = std::vector<Bot>;
using Ticks = std::vector<actions>;

struct Scenario {
  Bots bots;
  Ticks ticks;
  Expectations expected_positions;
};

void run_scenario(const std::string& name, const Scenario& scenario) {
  assert::it(name, [&, scenario = scenario]() {
    // ----------------- sanity check
    assert::is_false(scenario.bots.empty());
    std::unordered_map<uint32_t, std::shared_ptr<Player<ROWS, COLS>>> players{};

    // ----------------- setup game
    GameState<std::pair<std::shared_ptr<Player<ROWS, COLS>>, std::string>, ROWS, COLS> game(nullptr);

    for (uint32_t i = 0; i != scenario.bots.size(); ++i) {
      auto& bot = scenario.bots[i];
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

    // ------------------ check results
    for (const auto& expectation : scenario.expected_positions) {
      auto& [uuid, pos, tick_alive] = expectation;
      assert::equal(players[uuid]->pos(), pos);
      assert::equal(players[uuid]->tick_alive(), tick_alive);
    }

    // ------------------ look if grid has same context as players
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
        assert::equal_unordered(grid_context, player_region_context[uuid]);

      for (const auto& r: player_region_context[uuid]) {
        std::cout << r.first << ", " << r.second << "   ";
      }
      std::cout << "\n";
      for (const auto r : grid_region_context[uuid]) {
        std::cout << r.first << ", " << r.second << "   ";
      }
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";




      player_region_context.erase(uuid);
    }

    for (const auto& [uuid, grid_context] : grid_trail_context) {
      assert::is_true(player_trail_context.find(uuid) != player_trail_context.end(), std::to_string(uuid) + " not found in player_trail_context");
      if (player_trail_context.find(uuid) != player_trail_context.end())
        assert::equal_unordered(grid_context, player_trail_context[uuid]);

      for (const auto& r: player_trail_context[uuid]) {
        std::cout << r.first << ", " << r.second << "   ";
      }
      std::cout << "\n";
      for (const auto r : grid_trail_context[uuid]) {
        std::cout << r.first << ", " << r.second << "   ";
      }
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";

      player_trail_context.erase(uuid);
    }

    assert::is_true(player_region_context.empty());
    assert::is_true(player_trail_context.empty());
  });
}

#endif // SPACE_TESTS_UTILS_H
