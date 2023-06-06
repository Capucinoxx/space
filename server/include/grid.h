#ifndef SPACE_GRID_H
#define SPACE_GRID_H

#include "tile_map.h"

template<uint32_t ROWS, uint32_t COLS>
class Grid {
public:
  using position = std::pair<uint32_t, uint32_t>;

private:
  std::array<std::array<TileMap, COLS>, ROWS> grid;


public:
  Grid() = default;

  TileMap& at(const position& pos) {
    return grid[pos.first][pos.second];
  }
};

#endif //SPACE_GRID_H