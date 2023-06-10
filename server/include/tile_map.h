#ifndef SPACE_TILE_MAP_H
#define SPACE_TILE_MAP_H

#include "player.h"
#include <mutex>

class TileMap {
public:
  enum stmt { IDLE, DEATH, STEP, COMPLETE };

private:
  uint8_t value;
  bool is_step;
  mutable std::mutex mu;

public:
  TileMap() : value{ 0 }, is_step{ false } { }

  stmt step(short id) {
    std::lock_guard<std::mutex> lock(mu);

    if (is_step)
      return stmt::DEATH;

    if (value == id)
      return stmt::COMPLETE;

    is_step = true;
    value = id;
    return stmt::STEP;
  }

  void take(short id) {
    std::lock_guard<std::mutex> lock(mu);
    value = id;
    is_step = false;
  }

  bool is_trail() const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return is_step;
  }

  void reset() noexcept {
    std::lock_guard<std::mutex> lock(mu);    
    clear();
  }

  uint8_t get_value() const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return value;
  }

private:
  void clear() noexcept {
    value = 0;
    is_step = false;
  }
};

#endif //SPACE_TILE_MAP_H
