#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"

#include <vector>

template<std::size_t ROWS, std::size_t COLS>
class player {
public:
  enum movement_type { UP, DOWN, LEFT, RIGHT };
  using position_type = std::pair<int, int>;

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

protected:
  virtual movement_type move() = 0;
};

#endif //SPACE_PLAYER_H
