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

  
    scenarios.insert({ "steal while capture - steal territory", {
      Spawns{ position{ 4, 5 }, position{ 9, 6 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } }, Bot{ id{ 2 }, position{ 9, 6 } } },
      Ticks{
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, DOWN  } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, DOWN  } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, DOWN  } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, DOWN  } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, LEFT  } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, LEFT  } },
        actions{ { id{ 1 }, UP    }, { id{ 2 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 4, 6 },
          alive{ 15 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 197 },
          trail_pos{},
          region_pos{ position{ 3, 4 }, position{ 3, 5 }, position{ 3, 6 }, 
                      position{ 4, 4 }, position{ 4, 5 }, position{ 4, 6 }, position{ 4, 7 }, position{ 4, 8 }, 
                      position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 }, position{ 5, 8 }, 
                      position{ 6, 5 }, position{ 6, 6 }, position{ 6, 7 }, position{ 6, 8 }, 
                      position{ 7, 5 }, position{ 7, 6 }, position{ 7, 7 }, position{ 7, 8 },
                      position{ 8, 5 }, position{ 8, 6 }, position{ 8, 7 }, position{ 8, 8 },
                      position{ 9, 5 }, position{ 9, 6 }, position{ 9, 7 }, position{ 9, 8 }}
      },
      expectation{
          id{ 2 },
          position{ 10, 6 },
          alive{ 15 },
          scores{ 9, 18, 27, 35, 42, 48, 53, 58, 63, 68, 73, 78, 83, 88, 154 },
          trail_pos{},
          region_pos{ position{  9, 2 }, position{  9, 3 }, position{  9, 4 },
                      position{ 10, 2 }, position{ 10, 3 }, position{ 10, 4 }, position{ 10, 5 }, position{ 10, 6 }, position{ 10, 7 },
                      position{ 11, 2 }, position{ 11, 3 }, position{ 11, 4 }, position{ 11, 5 }, position{ 11, 6 },
                      position{ 12, 2 }, position{ 12, 3 }, position{ 12, 4 }, position{ 12, 5 }, position{ 12, 6 },
                      position{ 13, 2 }, position{ 13, 3 }, position{ 13, 4 }, position{ 13, 5 }, position{ 13, 6 }}
      }
    }
    
    }});

    scenarios.insert({ "capture and steal - steal territory", {
      Spawns{ position{ 1, 4 }, position{ 6, 5 } },
      Bots{ Bot{ id{ 1 }, position{ 1, 4 } }, Bot{ id{ 2 }, position{ 6, 5 } } },
      Ticks{
        actions{ { id{ 2 }, LEFT }, { id{ 1 }, UP    } },
        actions{ { id{ 2 }, LEFT }, { id{ 1 }, UP    } }, 
        actions{ { id{ 2 }, LEFT }, { id{ 1 }, RIGHT } },
        actions{ { id{ 2 }, DOWN }, { id{ 1 }, RIGHT } },
        actions{ { id{ 2 }, DOWN }, { id{ 1 }, RIGHT } },
        actions{ { id{ 2 }, RIGHT}, { id{ 1 }, DOWN  } },
        actions{ { id{ 2 }, RIGHT}, { id{ 1 }, DOWN  } },
        actions{ { id{ 2 }, UP   }, { id{ 1 }, DOWN  } },
        actions{ { id{ 2 }, RIGHT}, { id{ 1 }, LEFT  } }, 
        actions{ { id{ 2 }, RIGHT}, { id{ 1 }, LEFT  } }
      },
      Expectations{
        expectation{
          id{ 1 },
          position{ 2, 5 },
          alive{ 10 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 127 },
          trail_pos{},
          region_pos{ position{ 0, 3 }, position{ 0, 4 }, position{ 0, 5 },
                      position{ 1, 2 }, position{ 1, 3 }, position{ 1, 4 }, position{ 1, 5 },
                      position{ 2, 2 }, position{ 2, 3 }, position{ 2, 4 }, position{ 2, 5 },
                      position{ 3, 2 }, position{ 3, 3 }, position{ 3, 4 }, position{ 3, 5 },  
                      position{ 4, 2 }, position{ 4, 3 }, position{ 4, 4 }, position{ 4, 5 }}
      },
      expectation{
          id{ 2 },
          position{ 7, 6 },
          alive{ 10 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 99, 113, 127  },
          trail_pos{},
          region_pos{ position{ 3, 6 }, position{ 3, 7 },   
                      position{ 4, 6 }, position{ 4, 7 },
                      position{ 5, 4 }, position{ 5, 5 }, position{ 5, 6 }, position{ 5, 7 },
                      position{ 6, 4 }, position{ 6, 5 }, position{ 6, 6 }, 
                      position{ 7, 4 }, position{ 7, 5 }, position{ 7, 6 }}
      }
    }
    
    }});

    scenarios.insert({ "u pattern - steal territory", {
      Spawns{ position{ 4, 5 }, position{ 11, 2 } },
      Bots{ Bot{ id{ 1 }, position{ 4, 5 } }, Bot{ id{ 2 }, position{ 11, 2 } } },
      Ticks{

        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, DOWN  } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, LEFT  } }, 
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, LEFT  } }, 
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, LEFT  } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, LEFT  } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, LEFT  } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, LEFT  } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, UP    } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, UP    }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, LEFT  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, RIGHT }, { id{ 2 }, RIGHT } }, 
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } },
        actions{ { id{ 1 }, DOWN  }, { id{ 2 }, RIGHT } }
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
                      position{ 8, 2 }, position{ 8, 3 }, position{ 8, 4 }, position{ 8, 5 }, position{ 8, 6 }, position{ 8, 7 }, position{ 8, 8 }} 
      },
      expectation{
          id{ 2 },
          position{ 29, 1 },
          alive{ 33 },
          scores{ 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 177, 201, 225, 249, 272, 294, 315, 335, 354, 373, 392, 411, 427, 443, 459, 475, 491, 507, 523, 539 },
          trail_pos{ position{ 13, 1 }, position{ 14, 1 }, position{ 15, 1 }, position{ 16, 1 }, position{ 17, 1 }, position{ 18, 1 }, position{ 19, 1 }, position{ 20, 1 }, 
                     position{ 21, 1 }, position{ 22, 1 }, position{ 23, 1 }, position{ 24, 1 }, position{ 25, 1 }, position{ 26, 1 }, position{ 27, 1 }, position{ 28, 1 }, position{ 29, 1 }},
          region_pos{ position{ 5, 1 },
                      position{ 6, 1 }, 
                      position{ 7, 1 }, 
                      position{ 8, 1 }, 
                      position{ 9, 1 }, position{ 9, 2 }, position{ 9, 3 }, 
                      position{10, 1 }, position{10, 2 }, position{10, 3 },
                      position{11, 1 }, position{11, 2 }, position{11, 3 }, 
                      position{12, 1 }, position{12, 2 }, position{12, 3 }}
      }
    }
    
    }});
  }
};


#endif //SPACE_TESTS_STEAL_TERRITORY_H