#include "tile_map.h"

#include <iostream>

std::pair<tile_map::stmt, uint32_t> tile_map::step(uint32_t id) noexcept {
  auto old_owner = current_owner;

  if (stepping && current_owner != id && current_owner != 0) {
    current_owner = id;
    stepping = true;
    return { stmt::DEATH, old_owner };
  }

  if (current_owner == id)
    return { stmt::COMPLETE, old_owner };

  stepping = true;
  current_owner = id;
  return { stmt::STEP, old_owner };
}

std::pair<tile_map::stmt, uint32_t> tile_map::take(uint32_t id) noexcept {
  auto statement = stmt::STEP;

  if (current_owner != id && current_owner != 0 && stepping)
    statement = stmt::DEATH;

  auto old_owner = current_owner;
  current_owner = id;
  stepping = false;
  return { statement, old_owner };
}

uint32_t tile_map::owner() const noexcept {
  return current_owner;
}

bool tile_map::is_step() const noexcept {
  return stepping;
}

void tile_map::reset(uint32_t id) noexcept {
  if (current_owner == id)
    clear();
}

void tile_map::clear() noexcept {
  current_owner = 0;
  stepping = false;
}