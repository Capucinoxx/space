#ifndef SPACE_TESTS_TELEPORTATION_H
#define SPACE_TESTS_TELEPORTATION_H

#include "scenario.h"

struct teleportation_scenarios : scenarios_category {
public:
  void init() override {
    scenarios.insert({ "teleport on region position", {
      Spawns{ position{ 3, 3 } },
      Bots{ Bot{ id{ 1 }, position{ 3, 3 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, TELEPORT(2, 2) } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 2, 2 },
          alive{ 4 },
          scores{ 9, 18, 27, 36 },
          trail_pos{},
          region_pos{ position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 },
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 },
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 } }
        }
      }
    } });

    scenarios.insert({ "teleporting two cnosecutive ticks", {
      Spawns{ position{ 3, 3 } },
      Bots{ Bot{ id{ 1 }, position{ 3, 3 } } },
      Ticks{
        actions{ { id{ 1 }, TELEPORT(2, 2) } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, TELEPORT(4, 4) } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 4, 4 },
          alive{ 10 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90 },
          trail_pos{},
          region_pos{ position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 },
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 },
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 } }
        }
      }
    } });

    scenarios.insert( { "teleport second time after 8 ticks", {
      Spawns{ position{ 3, 3 } },
      Bots{ Bot{ id{ 1 }, position{ 3, 3 } } },
      Ticks{
        actions{ { id{ 1 }, TELEPORT(2, 2) } },
        actions{ { id{ 1 }, TELEPORT(4, 4) } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 2, 2 },
          alive{ 2 },
          scores{ 9, 18 },
          trail_pos{},
          region_pos{ position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 },
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 },
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 } }
        }
      }
    } });
  }
};

#endif //SPACE_TESTS_TELEPORTATION_H