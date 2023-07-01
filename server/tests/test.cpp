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

struct Tick {
  std::vector<std::pair<uint32_t, direction>> actions;
};

struct Scenario {
  std::vector<Bot> bots;
  std::vector<Tick> ticks;

  std::vector<std::pair<uint32_t, position>> expected_positions;
};

int main() {
  std::unordered_map<std::string, Scenario> scenarios;

  scenarios.insert({ "two bots", 
    {
      { Bot{ 1, { 10, 10 } } },
      {
        Tick{{ { 1, UP } }},
        Tick{{ { 1, UP } }},
        Tick{{ { 1, UP } }},
      },
      { { 1, { 10, 13 } } },
    }
  });

  for (auto& [name, scenario] : scenarios) {
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
          for (const auto& [uuid, dir] : tick.actions) {
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

  return 0;
}