#ifndef SPACE_TESTS_UTILS_H
#define SPACE_TESTS_UTILS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

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
std::ostream& operator<<(std::ostream& os, const position& pos) {
  os << "(" << pos.first << ", " << pos.second << ")";
  return os;
}

struct Bot {
  uint32_t uuid; // < unique id
  position spawn_pos;
};

using action = std::pair<uint32_t, direction>;
using actions = std::vector<action>;
using expectation = std::pair<uint32_t, position>;
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
      auto player = game.register_player("", i, { 0.0, 0.0, 0.0 }, 0.0, bot.spawn_pos);

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
    for (const auto& [uuid, pos] : scenario.expected_positions) {
      assert::equal(players[uuid]->pos(), pos);
    }
  });
}

#endif // SPACE_TESTS_UTILS_H
