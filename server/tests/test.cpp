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
        scores{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
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
        scores{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
        trail_pos{},
        region_pos{ position{ 13, 13 }, position{ 13, 14 }, position{ 13, 15 },
                    position{ 14, 13 }, position{ 14, 14 }, position{ 14, 15 },
                    position{ 15, 13 }, position{ 15, 14 }, position{ 15, 15 } }
      }
    }
  }});

  scenarios.insert({ "weird pattern - 1", {
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
        scores{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
                0, 0 ,0 ,0 ,0 ,0 ,0 ,0 },
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

  scenarios.insert({ "weird pattern - 2", {
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
        scores{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0 ,0 ,0 ,0 ,0 },
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

  scenarios.insert({ "kill player on spawn", { 
    Spawns{ position{ 5, 5 }, position{ 6, 6 }, position{ 10, 10 } },
    Bots{ Bot{ id{ 1 }, position{ 5, 5 } }, Bot{ id{ 2 }, position{ 6, 6 } } },
    Ticks{},
    Expectations{
      expectation{
        id{ 1 },
        position{ 10 ,10 },
        alive{ 0 },
        scores{},
        trail_pos{},
        region_pos{ position{ 9, 9 }, position{ 9, 10 }, position{ 9, 11 },
                    position{ 10, 9 }, position{ 10, 10 }, position{ 10, 11 },
                    position{ 11, 9 }, position{ 11, 10 }, position{ 11, 11 } }
      },
      expectation{
        id{ 2 },
        position{ 6, 6 },
        alive{ 0 },
        scores{},
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
        scores{ 0, 0, 0, 0, 0, 0 },
        trail_pos{},
        region_pos{ position{ 13, 13 }, position{ 13, 14 }, position{ 13, 15 },
                    position{ 14, 13 }, position{ 14, 14 }, position{ 14, 15 },
                    position{ 15, 13 }, position{ 15, 14 }, position{ 15, 15 } }
      },
      expectation{
        id{ 2 },
        position{ 4, 1 },
        alive{ 0 },
        scores{ 0, 0, 0, 0, 0, 0 },
        trail_pos{},
        region_pos{ position{ 3, 0 }, position{ 3, 1 }, position{ 3, 2 },
                    position{ 4, 0 }, position{ 4, 1 }, position{ 4, 2 },
                    position{ 5, 0 }, position{ 5, 1 }, position{ 5, 2 } }
      }
    }
  }});

  scenarios.insert({ "take region of other player on spawn", {
    Spawns{ position{ 2, 2 }, position{ 4, 2 }, position{ 14, 14 } },
    Bots{ Bot{ id{ 1 }, position{ 2, 2 } }, Bot{ id{ 2 }, position{ 4, 2 } } },
    Ticks{},
    Expectations{
      expectation{
        id{ 1 },
        position{ 2, 2 },
        alive{ 0 },
        scores{},
        trail_pos{},
        region_pos{ position{ 1, 1 }, position{ 1, 2 }, position{ 1, 3 },
                    position{ 2, 1 }, position{ 2, 2 }, position{ 2, 3 } }
      },
      expectation{
        id{ 2 },
        position{ 4, 2 },
        alive{ 0 },
        scores{},
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