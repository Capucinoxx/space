#ifndef SPACE_TILE_MAP_H
#define SPACE_TILE_MAP_H

#include <utility>
#include <cstdint>

class tile_map {
public:
  enum stmt { IDLE, DEATH, STEP, COMPLETE };

private:
  uint32_t current_owner;
  uint32_t current_walker;

public:
  tile_map() : current_owner{ 0 }, current_walker{ 0 } { }

  std::pair<stmt, uint32_t> step(uint32_t id) noexcept;

  std::pair<stmt, uint32_t> take(uint32_t id) noexcept;

  uint32_t owner() const noexcept;
  uint32_t walker() const noexcept;

  bool is_step() const noexcept;

  void reset(uint32_t id) noexcept;

  void clear() noexcept;
};

#endif //SPACE_TILE_MAP_H
