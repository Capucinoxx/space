#ifndef SPACE_TESTS_STEAL_TERRITORY_H
#define SPACE_TESTS_STEAL_TERRITORY_H

#include <vector>
#include <unordered_map>

#include "scenario.h"

struct steal_territory_scenarios : scenarios_category {
public:
  void init() {
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
  }
};


#endif //SPACE_TESTS_STEAL_TERRITORY_H