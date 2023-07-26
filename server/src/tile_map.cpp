#include "tile_map.h"

#include <iostream>

std::pair<tile_map::stmt, uint32_t> tile_map::step(uint32_t id) noexcept {
  auto old_stepper = current_stepper;

  if (is_step()) {
    current_stepper = id;
    return { stmt::DEATH, old_stepper };
  }

  if (current_owner == id)
    return { stmt::COMPLETE, old_stepper };

  current_stepper = id;
  return { stmt::STEP, old_stepper };
}

std::pair<tile_map::stmt, uint32_t> tile_map::take(uint32_t id) noexcept {
  auto statement = stmt::STEP;
  auto old = current_owner;
  
  if (current_stepper != id && current_stepper != 0) {
    statement = stmt::DEATH;
    old = current_stepper;
  }
    
  current_stepper = 0;
  current_owner = id;

  return { statement, old };
}

uint32_t tile_map::owner() const noexcept {
  return current_owner;
}

uint32_t tile_map::walker() const noexcept {
  return current_stepper;
}

bool tile_map::is_step() const noexcept {
  return current_stepper != 0;
}

void tile_map::reset(uint32_t id) noexcept {
  if (current_owner == id)
    current_owner = 0;

  if (current_stepper == id)
    current_stepper = 0;
}

void tile_map::clear() noexcept {
  current_owner = 0;
  current_stepper = 0;
}