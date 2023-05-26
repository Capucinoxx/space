#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include <vector>

#include "player.h"
#include "tile_map.h"

template<std::size_t ROWS, std::size_t COLS>
class map {
private:
  tile_map fields[ROWS][COLS];
  std::vector<player *> players;
  std::vector<player::position_type> spawns;

public:
  map();
  bool update() {
    for (const player& p : players) {
      tile_map::stmt res = p.update(fields);
      if (res == tile_map::stmt::DEATH)
        kill(p);
      else if(res == tile_map::stmt::COMPLETE)
        complete_area(p);
    }
  }

  bool spawn(player *p);
  bool kill(player *p);
  bool complete_area(player *p);

  bool is_in_bounds(player::position_type pos);
};

#endif //SPACE_MAP_H
