#ifndef SPACE_TESTS_ZONE_COMPLETION_H
#define SPACE_TESTS_ZONE_COMPLETION_H

#include <vector>
#include <unordered_map>

#include "scenario.h"


struct zone_completion_scenarios : scenarios_category {
public:
  void init() {
    scenarios.insert({ "weird pattern 1 - zone completion", {
      Spawns{ position{ 25, 15 } },
      Bots{ Bot{ id{ 1 }, position{ 25, 15 } } },
      Ticks{
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 24, 14 },
          alive{ 38 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 234, 243, 252, 261, 270, 279, 288, 297, 306, 315, 324, 333 },
          trail_pos{},
          region_pos{ position{ 23, 10 }, position{ 23, 11 }, position{ 23, 12 }, position{ 23, 13 }, position{ 23, 14 }, position{ 23, 15 }, position{ 23, 16 }, position{ 23, 17 },
                      position{ 24, 10 }, position{ 24, 11 }, position{ 24, 12 }, position{ 24, 13 }, position{ 24, 14 }, position{ 24, 15 }, position{ 24, 16 }, position{ 24, 17 },
                                          position{ 25, 11 }, position{ 25, 12 }, position{ 25, 13 }, position{ 25, 14 }, position{ 25, 15 }, position{ 25, 16 }, position{ 25, 17 },
                                          position{ 26, 11 }, position{ 26, 12 }, position{ 26, 13 }, position{ 26, 14 }, position{ 26, 15 }, position{ 26, 16 }, position{ 26, 17 },
                      position{ 27, 10 }, position{ 27, 11 }, position{ 27, 12 }, position{ 27, 13 }, position{ 27, 14 }, position{ 27, 15 }, position{ 27, 16 }, position{ 27, 17 },
                      position{ 28, 10 }, position{ 28, 11 }, position{ 28, 12 }, position{ 28, 13 }, position{ 28, 14 }, position{ 28, 15 }, position{ 28, 16 }, position{ 28, 17 },
                      position{ 29, 10 }, position{ 29, 11 }, position{ 29, 12 }, position{ 29, 13 } }
      }
    }
    }});

    scenarios.insert({ "weird pattern 2 - zone completion", {
      Spawns{ position{ 25, 15 } },
      Bots{ Bot{ id{ 1 }, position{ 25, 15 } } },
      Ticks{
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 24, 14 },
          alive{ 16 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 203 },
          trail_pos{},
          region_pos{ position{ 26, 16 }, position{ 24, 16 }, position{ 23, 12 },
                      position{ 26, 15 }, position{ 24, 15 }, position{ 23, 11 }, 
                      position{ 26, 14 }, position{ 24, 14 }, position{ 22, 13 }, 
                      position{ 25, 16 }, position{ 24, 13 }, position{ 22, 12 }, 
                      position{ 25, 15 }, position{ 24, 12 }, position{ 22, 11 }, 
                      position{ 25, 14 }, position{ 23, 14 }, position{ 22, 10 },
                      position{ 25, 13 }, position{ 23, 13 }, position{ 21, 12 }, position{ 21, 11 },  position{ 21, 10 }}
      }
    }
    }});

    scenarios.insert({ "weird pattern 3 - zone completion", {
      Spawns{ position{ 2, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 3 },
          alive{ 22 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 281 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
                      position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, position{ 2, 7 }, 
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, 
                      position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, 
                      position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, 
                      position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }}
      }
    }
    }});

    scenarios.insert({ "weird pattern 4 - zone completion", {
      Spawns{ position{ 4, 5 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } } },
      Ticks{
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 5, 5 },
          alive{ 27 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 234, 345 },
          trail_pos{},
          region_pos{ position{ 1, 4 }, position{ 1, 5 }, position{ 1, 6 },
                      position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, position{ 2, 7 }, 
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, position{ 3, 8 }, 
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, position{ 4, 8 }, 
                      position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 }, 
                      position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 },
                      position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 }}
      }
    }
    }});

    scenarios.insert({ "weird pattern 5 - zone completion", {
      Spawns{ position{ 3, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 3, 2 } } },
      Ticks{
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, 
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 4, 1 },
          alive{ 42 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 234, 243, 252, 261, 270, 279, 288, 297, 306, 315, 324, 333, 342, 351, 360, 369, 543 },
          trail_pos{},
          region_pos{ position{ 1, 0 }, position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 }, position{ 1, 4 }, 
                      position{ 2, 0 }, position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, 
                      position{ 3, 0 }, position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, 
                      position{ 4, 0 }, position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, 
                      position{ 5, 0 }, position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, 
                      position{ 6, 0 }, position{ 6, 1 }, position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, 
                      position{ 7, 0 }, position{ 7, 1 }, position{ 7, 2 }, position{ 7, 3 }, position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 },
                      position{ 8, 3 }, position{ 8, 4 }, position{ 8, 5 }, position{ 8, 6 },
                      position{ 9, 3 }, position{ 9, 4 }, position{ 9, 5 }, position{ 9, 6 }}
      }
    }
    }});

    scenarios.insert({ "weird pattern 6 - zone completion", {
      Spawns{ position{ 4, 5 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, 
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, 
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, 
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }

      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 4, 4 },
          alive{ 45 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 234, 243, 252, 261, 270, 279, 288, 297, 306, 315, 324, 333, 342, 351, 360, 369, 378, 387, 396, 588 },
          trail_pos{},
          region_pos{ position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, 
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, 
                      position{ 5, 0 }, position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 },
                      position{ 6, 0 }, position{ 6, 1 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }, position{ 6, 8 }, position{ 6, 9 }, 
                      position{ 7, 0 }, position{ 7, 1 }, position{ 7, 3 }, position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 }, position{ 7, 8 }, position{ 7, 9 }, 
                      position{ 8, 0 }, position{ 8, 1 }, position{ 8, 2 }, position{ 8, 4 }, position{ 8, 5 }, position{ 8, 6 },
                      position{ 9, 0 }, position{ 9, 1 }, position{ 9, 2 }, position{ 9, 4 }, position{ 9, 5 }, position{ 9, 6 },
                      position{10, 0 }, position{10, 1 }, position{10, 2 }, position{10, 3 }, position{10, 4 }, position{10, 5 }, 
                      position{11, 0 }, position{11, 1 }, position{11, 2 }, position{11, 3 }, position{11, 4 }, position{11, 5 }, 
                      }
      }
    }
    }});

    scenarios.insert({ "rectangle pattern 1 - zone completion", {
      Spawns{ position{ 2, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } } },
      Ticks{
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP   } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP   } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP   } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP   } },
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 3 },
          alive{ 32 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 234, 243, 252, 261, 270, 279, 420 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2  }, position{ 1, 3  }, position{ 1, 4  }, position{ 1, 5  }, position{ 1, 6  }, position{ 1, 7  }, position{ 1, 8  },
                      position{ 1, 9 }, position{ 1, 10 }, position{ 1, 11 }, position{ 1, 12 }, position{ 1, 13 }, position{ 1, 14 }, position{ 1, 15 }, position{ 1, 16 }, position{ 1, 17 },
                      position{ 2, 1 }, position{ 2, 2  }, position{ 2, 3  }, position{ 2, 4  }, position{ 2, 5  }, position{ 2, 6  }, position{ 2, 7  }, position{ 2, 8  },
                      position{ 2, 9 }, position{ 2, 10 }, position{ 2, 11 }, position{ 2, 12 }, position{ 2, 13 }, position{ 2, 14 }, position{ 2, 15 }, position{ 2, 16 }, position{ 2, 17 },
                      position{ 3, 1 }, position{ 3, 2  }, position{ 3, 3  }, position{ 3, 4  }, position{ 3, 5  }, position{ 3, 6  }, position{ 3, 7  }, position{ 3, 8  },
                      position{ 3, 9 }, position{ 3, 10 }, position{ 3, 11 }, position{ 3, 12 }, position{ 3, 13 }, position{ 3, 14 }, position{ 3, 15 }, position{ 3, 16 }, position{ 3, 17 }}
      }
    }
    }});

    scenarios.insert({ "rectangle pattern 2 - zone completion", {
      Spawns{ position{ 2, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } } },
      Ticks{
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 3 },
          alive{ 12 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 153 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
                      position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 },  
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, 
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 },
                      position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, 
                      position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, 
                      position{ 7, 2 }, position{ 7, 3 }, position{ 7, 4 } }
      }
    }
    }});

    scenarios.insert({ "square pattern - zone completion", {
      Spawns{ position{ 4, 5 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 6 },
          alive{ 18 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 229 },
          trail_pos{},
          region_pos{ position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, position{ 2, 7 }, 
                      position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, 
                      position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, 
                      position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, 
                      position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 } }
      }
    }
    }});

    scenarios.insert({ "U pattern - zone completion", {
      Spawns{ position{ 2, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } } },
      Ticks{
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 7, 3 },
          alive{ 40 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 150, 171, 192, 213, 234, 255, 276, 297, 318, 339, 360, 381, 402, 423, 444, 465, 486, 507, 528, 549, 648, 693, 738, 783, 828, 873, 918, 963, 1018 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 }, position{ 1, 4 }, position{ 1, 5 }, position{ 1, 6 }, position{ 1, 7 },
                      position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, position{ 2, 7 }, 
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, 
                      position{ 4, 1 }, position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, 
                      position{ 5, 1 }, position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, 
                      position{ 6, 1 }, position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }, 
                      position{ 7, 1 }, position{ 7, 2 }, position{ 7, 3 }, position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 } }
      }
    }
    }});

    scenarios.insert({ "U pattern 2 - zone completion", {
      Spawns{ position{ 4, 5 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, 
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 8, 6 },
          alive{ 33 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 180, 189, 198, 207, 216, 225, 337, 377, 417, 457, 497, 537, 577, 625 },
          trail_pos{},
          region_pos{ position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, position{ 3, 8 }, 
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, position{ 4, 8 }, 
                      position{ 5, 2 }, position{ 5, 3 }, position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 }, 
                      position{ 6, 2 }, position{ 6, 3 }, position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }, position{ 6, 8 }, 
                      position{ 7, 2 }, position{ 7, 3 }, position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 }, position{ 7, 8 }, 
                      position{ 8, 2 }, position{ 8, 3 }, position{ 8, 4 }, position{ 8, 5 }, position{ 8, 6 }, position{ 8, 7 }, position{ 8, 7 }} 
      }
    }
    }});

    scenarios.insert({ "L pattern - zone completion", {
      Spawns{ position{ 2, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 2, 2 } } },
      Ticks{
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } },
        actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, DOWN  } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } }, actions{ { id{ 1 }, RIGHT } },
        actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } },
        actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, LEFT  } }, actions{ { id{ 1 }, UP    } }, actions{ { id{ 1 }, UP    } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 3, 3 },
          alive{ 20 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144, 153, 162, 171, 258 },
          trail_pos{},
          region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 }, position{ 1, 4 }, position{ 1, 5 }, position{ 1, 6 }, position{ 1, 7 },
                      position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 }, position{ 2, 6 }, position{ 2, 7 }, 
                      position{ 3, 1 }, position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, position{ 3, 7 }, 
                      position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, 
                      position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, 
                      position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }, 
                      position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 } }
      }
    }
    }});
  }
};



#endif //SPACE_TESTS_ZONE_COMPLETION_H