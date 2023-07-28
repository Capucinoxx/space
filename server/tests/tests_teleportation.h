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

    scenarios.insert({ "teleport on trail", { 
      Spawns{ position{ 2, 2 }, position{ 5, 2 }, position{ 14, 14 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } }, Bot{ id{ 2 }, position{ 5, 2 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, TELEPORT(3, 2) }, { id{ 2 }, LEFT } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 2, 6 },
          alive{ 5 },
          scores{ 9, 18, 27, 36, 45 },
          trail_pos{ position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 } },
          region_pos{
            position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
            position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 },
            position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 },
          }
        },
        expectation{
          id{ 2 },
          position{ 0, 2 },
          alive{ 5 },
          scores{ 9, 18, 27, 36, 45 },
          trail_pos{ position{ 3, 2 }, position{ 2, 2 }, position{ 1, 2 }, position{ 0, 2 } },
          region_pos{
            position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 },
            position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 },
            position{ 6, 1 }, position{ 6, 2 }, position{ 6, 3 },
          }
        }
      }
     } });
  }
};

#endif //SPACE_TESTS_TELEPORTATION_H