#ifndef SPACE_TILE_MAP_H
#define SPACE_TILE_MAP_H

#include "player.h"

class TileMap {
public:
  enum stmt { IDLE, DEATH, STEP, COMPLETE };

private:
  uint8_t value;
  bool is_step;

public:
  TileMap() : value{ 0 }, is_step{ false } { }

  stmt step(short id) {
    if (is_step)
      return stmt::DEATH;

    if (value == id)
      return stmt::COMPLETE;

    is_step = true;
    value = id;
    return stmt::STEP;
  }

  void set(short id) {
    value = id;
  }

  bool is_trail() const noexcept { return is_step; }
  void clear() noexcept { is_step = false; }
  void reset() noexcept {
    value = 0;
    clear();
  }
};

#endif //SPACE_TILE_MAP_H
