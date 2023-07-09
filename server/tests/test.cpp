#include <vector>
#include <unordered_map>


#include "utils.h"

int main() {
  std::unordered_map<std::string, Scenario> scenarios;

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
        scores{ 0, 0, 0 },
        trail_pos{ position{ 10, 8 }, position{ 10, 7 } },
        region_pos{ position{ 9, 9 }, position{ 9, 10 }, position{ 9, 11 },  
                    position{ 10, 9 }, position{ 10, 10 }, position{ 10, 11 },
                    position{ 11, 9 }, position{ 11, 10 }, position{ 11, 11 } }
      }, 
    },
  }});

  scenarios.insert({ "kill at the same tick, without score", {
    Spawns{ position{ 3, 3 }, position{ 5, 7 } },
    Bots{ Bot{ id{ 1 }, position{ 3, 3 } }, Bot{ id{ 2 }, position{ 5, 7 } } },
    Ticks{
      actions{ { id{ 1 }, DOWN }, { id{ 2 }, UP } },
      actions{ { id{ 1 }, DOWN }, { id{ 2 }, UP } },
      actions{ { id{ 1 }, RIGHT }, { id{ 2 }, LEFT } },
    },
    Expectations{
      expectation{
        id{ 2 },
        position{ 4, 5 },
        alive{ 3 },
        scores{ 0, 0, 0 },
        trail_pos{ position{ 5, 5 }, position{ 4, 5 } },
        region_pos{ position{ 4, 6 }, position{ 4, 7 }, position{ 4, 8 },
                    position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 },
                    position{ 6, 6 }, position{ 6, 7 }, position{ 6, 8 } }
      },
    }
  }});

  for (auto& [name, scenario] : scenarios)
    scenario.run(name);
  
  return 0;
}