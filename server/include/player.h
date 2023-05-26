#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"

#include <vector>

template<std::size_t ROWS, std::size_t COLS>
class player {
public:
  enum { maximum_name_size = 15 };

  enum movement_type { UP, DOWN, LEFT, RIGHT };
  using position_type = std::pair<int, int>;
  using trail_iterator = typename std::vector<movement_type>::iterator;
  using const_trail_iterator = typename std::vector<movement_type>::const_iterator;

private:
  short identifier;
  std::string username;
  position_type location;
  std::vector<movement_type> trail = new std::vector<movement_type>(ROWS * COLS);

public:
  virtual tile_map::stmt update() = 0;
  virtual ~player();

  short id() const noexcept { return identifier; }
  std::string name() const noexcept { return username; }

  position_type position() const noexcept {
    return location;
  }

  void set_position(position_type pos) {
    location = pos;
  }

  trail_iterator trail_begin() noexcept {
    return trail.begin();
  }

  trail_iterator trail_end() noexcept {
    return trail.end();
  }

  const_trail_iterator trail_begin() const noexcept {
    return trail.begin();
  }

  const_trail_iterator trail_end() const noexcept {
    return trail.end();
  }

protected:
  virtual movement_type move() = 0;
};

#endif //SPACE_PLAYER_H
