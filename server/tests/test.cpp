#include <vector>
#include <unordered_map>
#include <memory>


#include "scenario.h"

#include "tests_zone_completion.h"
#include "tests_kill.h"

int main() {
  std::unordered_map<std::string, Scenario> scenarios;

  auto insert_scenarios = [&scenarios](std::unique_ptr<scenarios_category> category) {
    category->init();
    scenarios.insert(category->begin(), category->end());
  };

  insert_scenarios(std::make_unique<zone_completion_scenarios>());
  insert_scenarios(std::make_unique<kill_scenarios>());

  scenarios.insert({ "dummy test", {
    Spawns{ position{ 10, 10 } },
    Bots{ Bot{ id{ 1 }, position{ 10, 10 } } },
    Ticks{
      actions{ { id{ 1 }, UP } },
      actions{ { id{ 1 }, UP } },
      actions{ { id{ 1 }, UP } },
    },
    Expectations{ 
      expectation{ 
        id{ 1 }, 
        position{ 10, 7 }, 
        alive{ 3 }, 
        scores{ 9, 18, 27 },
        trail_pos{ position{ 10, 8 }, position{ 10, 7 } },
        region_pos{ position{ 9, 9 }, position{ 9, 10 }, position{ 9, 11 },  
                    position{ 10, 9 }, position{ 10, 10 }, position{ 10, 11 },
                    position{ 11, 9 }, position{ 11, 10 }, position{ 11, 11 } }
      }, 
    },
  }});



  scenarios.insert({ "take region of other player on spawn", {
    Spawns{ position{ 2, 2 }, position{ 4, 2 }, position{ 14, 14 } },
    Bots{ Bot{ id{ 1 }, position{ 2, 2 } }, Bot{ id{ 2 }, position{ 4, 2 } } },
    Ticks{ actions{ { id{ 1 }, UP }, { id{ 2 }, UP } } },
    Expectations{
      expectation{
        id{ 1 },
        position{ 2, 1 },
        alive{ 1 },
        scores{ 6 },
        trail_pos{},
        region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
                    position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 } }
      },
      expectation{
        id{ 2 },
        position{ 4, 1 },
        alive{ 1 },
        scores{ 9 },
        trail_pos{},
        region_pos{ position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 },
                    position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 },
                    position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 } }
      }
    }
  }});

  for (auto& [name, scenario] : scenarios)
    scenario.run(name);
  
  return 0;
}