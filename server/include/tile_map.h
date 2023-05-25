#ifndef SPACE_TILE_MAP_H
#define SPACE_TILE_MAP_H

#include "player.h"

class tile_map {
public:
  enum stmt { IDLE, DEATH, STEP, COMPLETE };

private:
  short value;
  bool is_step;

public:
  tile_map() : value{ 0 }, is_step{ false } { }

  player::movement_type step(short id) {
    if (is_step)
      return stmt::DEATH;

    if (value == id)
      return stmt::COMPLETE;

    is_step = true;
    value = id;
    return stmt::STEP;
  }

  bool is_trail() const noexcept { return is_step; }
  void clear() noexcept { is_step = false; }
  void reset() noexcept {
    value = 0;
    clear();
  }
};

#endif //SPACE_TILE_MAP_H
