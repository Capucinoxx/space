#ifndef SPACE_TESTS_KILL_H
#define SPACE_TESTS_KILL_H

#include "scenario.h"


struct kill_scenarios : scenarios_category {
public:
  void init() override {
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
          scores{ 9, 18, 63 },
          trail_pos{ position{ 5, 5 }, position{ 4, 5 } },
          region_pos{ position{ 4, 6 }, position{ 4, 7 }, position{ 4, 8 },
                      position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 },
                      position{ 6, 6 }, position{ 6, 7 }, position{ 6, 8 } }
        },
      }
    }});

    scenarios.insert({ "encirclement elimination", {
      Spawns{ position{ 7, 3 }, position{ 3, 3 }, position{ 14, 14 } },
      Bots{ Bot{ id{ 1 }, position{ 7, 3 } }, Bot{ id{ 2 }, position{ 3, 3 } } },
      Ticks{
        actions{ { id{ 1 }, UP   }, { id{ 2 }, UP } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, UP } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, DOWN } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, DOWN } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, DOWN } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, DOWN } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, UP   }, { id{ 2 }, LEFT } },
        actions{ { id{ 1 }, DOWN }, { id{ 2 }, UP } },
      },
      Expectations{
        expectation{
          id{ 2 },
          position{ 4, 4 },
          alive{ 18 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117 , 126, 135, 144, 153, 445 },
          trail_pos{},
          region_pos{ position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 },
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 },
                      position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 },
                      position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 },
                      position{ 6, 1 }, position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 },
                      position{ 7, 1 }, position{ 7, 2 }, position{ 7, 3 }, position{ 7, 4 }, position{ 7, 5 },
                      position{ 8, 1 }, position{ 8, 2 }, position{ 8, 3 }, position{ 8, 4 }, position{ 8, 5 },
                      position{ 9, 1 }, position{ 9, 2 }, position{ 9, 3 }, position{ 9, 4 }, position{ 9, 5 }}
        },
        expectation{
          id{ 1 },
          position{ 14, 14 },
          alive{ 0 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117 , 126, 135, 144, 153, 162 },
          trail_pos{},
          region_pos{ position{ 13, 13 }, position{ 13, 14 }, position{ 13, 15 },
                      position{ 14, 13 }, position{ 14, 14 }, position{ 14, 15 },
                      position{ 15, 13 }, position{ 15, 14 }, position{ 15, 15 } }
        }
      }
    }});

    scenarios.insert({ "kill player on spawn", { 
      Spawns{ position{ 5, 5 }, position{ 6, 6 }, position{ 10, 10 } },
      Bots{ Bot{ id{ 1 }, position{ 5, 5 } }, Bot{ id{ 2 }, position{ 6, 6 } } },
      Ticks{ actions{ { id{ 2 }, UP } } },
      Expectations{
        expectation{
          id{ 1 },
          position{ 10, 10 },
          alive{ 0 },
          scores{ 9 },
          trail_pos{},
          region_pos{ position{ 9, 9 }, position{ 9, 10 }, position{ 9, 11 },
                      position{ 10, 9 }, position{ 10, 10 }, position{ 10, 11 },
                      position{ 11, 9 }, position{ 11, 10 }, position{ 11, 11 } }
        },
        expectation{
          id{ 2 },
          position{ 6, 5 },
          alive{ 1 },
          scores{ 21 },
          trail_pos{},
          region_pos{ position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 },
                      position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 },
                      position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 } }
        }
      }
    }});

    scenarios.insert({ "kill player spawn on trail", {
      Spawns{ position{ 1, 1 }, position{ 1, 5 }, position{ 4, 1 }, position{ 14, 14 } },
      Bots{ Bot{ id{ 1 }, position{ 1, 1 } }, Bot{ id{ 2 }, position{ 1, 5 } } },
      Ticks{
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 14, 14 },
          alive{ 0 },
          scores{ 9, 18, 27, 36, 45, 54 },
          trail_pos{},
          region_pos{ position{ 13, 13 }, position{ 13, 14 }, position{ 13, 15 },
                      position{ 14, 13 }, position{ 14, 14 }, position{ 14, 15 },
                      position{ 15, 13 }, position{ 15, 14 }, position{ 15, 15 } }
        },
        expectation{
          id{ 2 },
          position{ 4, 1 },
          alive{ 0 },
          scores{ 9, 18, 27, 36, 45, 66 },
          trail_pos{},
          region_pos{ position{ 3, 0 }, position{ 3, 1 }, position{ 3, 2 },
                      position{ 4, 0 }, position{ 4, 1 }, position{ 4, 2 },
                      position{ 5, 0 }, position{ 5, 1 }, position{ 5, 2 } }
        }
      }
    }});

    scenarios.insert({ "kill player in territory", {
      Spawns{ position{ 2, 3 }, position{ 3, 6 }, position{ 8, 8 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 3 } }, Bot{ id{ 2 }, position{ 3, 6 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, UP    } },
        // actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } }

        actions{ { id{ 2 }, UP    }, { id{ 1 }, RIGHT } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 4 },
          alive{ 2 },
          scores{ 9, 30 },
          trail_pos{ },
          region_pos{ position{ 1, 2 }, position{ 1, 3 }, position{ 1, 4 },
                      position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 },
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 } }
        },
        expectation{
          id{ 2 },
          position{ 8, 8 },
          alive{ 0 },
          scores{ 9, 18 },
          trail_pos{},
          region_pos{ position{ 7, 7 }, position{ 7, 8 }, position{ 7, 9 },
                      position{ 8, 7 }, position{ 8, 8 }, position{ 8, 9 },
                      position{ 9, 7 }, position{ 9, 8 }, position{ 9, 9 } }
        }
      }
    }});


    scenarios.insert({ "kill player in territory 2", {
      Spawns{ position{ 2, 2 }, position{ 4, 7 }, position{ 14, 14 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } }, Bot{ id{ 2 }, position{ 4, 7 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, UP    } },

        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 4, 3 },
          alive{ 21 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 184, 212, 240, 268, 296, 324, 352, 392 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
                      position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 },
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 },
                      position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 },
                      position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 },
                      position{ 6, 1 }, position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 } },
        },
        expectation{
          id{ 2 },
          position{ 15, 14 },
          alive{ 0 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189 },
          trail_pos{},
          region_pos{ position{ 13, 13 }, position{ 13, 14 }, position{ 13, 15 },
                      position{ 14, 13 }, position{ 14, 14 }, position{ 14, 15 },
                      position{ 15, 13 }, position{ 15, 14 }, position{ 15, 15 } }
        }
      }
    } });
  }
};

#endif //SPACE_TESTS_KILL_H