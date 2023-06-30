#ifndef SPACE_TILE_MAP_H
#define SPACE_TILE_MAP_H

#include "player.h"
#include <mutex>

class TileMap {
public:
  enum stmt { IDLE, DEATH, STEP, COMPLETE };

private:
  uint32_t current_owner;
  bool is_step;
  mutable std::mutex mu;

public:
  TileMap() : current_owner{ 0 }, is_step{ false } { }

  stmt step(uint32_t id) noexcept {
    std::lock_guard<std::mutex> lock(mu);

    if (is_step) {
      current_owner = id;
      return stmt::DEATH;
    }

    if (current_owner == id)
      return stmt::COMPLETE;

    is_step = true;
    current_owner = id;
    return stmt::STEP;
  }

  bool has_someone_else(uint32_t id) const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return current_owner != id && current_owner != 0;
  }

  std::pair<stmt, uint32_t> take(uint32_t id) noexcept {
    std::lock_guard<std::mutex> lock(mu);
    auto statement = stmt::STEP;
    if (current_owner != id && current_owner != 0)
      statement = stmt::DEATH;

    auto old_owner = current_owner;
    current_owner = id;
    is_step = false;
    return { statement, old_owner };
  }

  uint32_t owner() const noexcept {
    std::lock_guard<std::mutex> lock(mu);
    return current_owner;
  }

private:
  void clear() noexcept {
    std::lock_guard<std::mutex> lock(mu);
    current_owner = 0;
    is_step = false;
  }
};

#endif //SPACE_TILE_MAP_H
