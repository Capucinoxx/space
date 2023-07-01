#ifndef SPACE_GRID_H
#define SPACE_GRID_H

#include <memory>

#include "tile_map.h"
#include "player.h"
template<uint32_t ROWS, uint32_t COLS>
class Grid {
public:
  using position = std::pair<uint32_t, uint32_t>;
  using player_ptr = std::shared_ptr<Player<ROWS, COLS>>;

private:
  std::array<std::array<TileMap, COLS>, ROWS> grid;


public:
  Grid() = default;

  TileMap& at(const position& pos) {
    return grid[pos.first][pos.second];
  }
  

  std::vector<uint32_t> fill_region(player_ptr p) {
    std::vector<uint32_t> killed{};

    p->for_each_trail([&](const position& pos) {
      at(pos).take(p->id());
      p->append_region({ pos });
    });
    p->clear_trail();

    std::array<std::array<bool, COLS>, ROWS> been{};
    std::vector<position> neighbors{};

    neighbors.reserve(ROWS > COLS ? ROWS : COLS);
    neighbors.push_back(p->pos());

    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_out_of_bounds(pos) || been[pos.first][pos.second] || grid[pos.first][pos.second].owner() == p->id())
        continue;

      been[pos.first][pos.second] = true;

      p->append_region({ pos });

      auto movements = {
        std::make_pair(1, 0),
        std::make_pair(-1, 0),
        std::make_pair(0, 1),
        std::make_pair(0, -1)
      };
      
      for (auto& movement : movements) {
        auto px = pos.first + movement.first;
        auto py = pos.second + movement.second;

        auto res = flood_fill(p->id(), { px, py }, been);
        p->append_region(res.second);
        killed.insert(killed.end(), res.first.begin(), res.first.end());

        neighbors.push_back({ px, py });
      }
    }

    return killed;
  }

    bool is_out_of_bounds(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS;
  }

private:


  std::pair<std::unordered_set<uint32_t>, std::vector<position>> flood_fill(uint32_t self_id, const position& pos, std::array<std::array<bool, COLS>, ROWS>& been) {
    if (is_out_of_bounds(pos) || been[pos.first][pos.second] || at(pos).owner() == self_id)
      return { {}, {} };

    bool surrounded = true;
    std::vector<position> neighbors{};
    neighbors.reserve(ROWS > COLS ? ROWS : COLS);

    std::vector<position> filled{};
    filled.reserve(ROWS > COLS ? ROWS : COLS);

    neighbors.push_back(pos);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_out_of_bounds(pos)) {
        surrounded = false;
        continue;
      }

      if (been[pos.first][pos.second])
        continue;

      been[pos.first][pos.second] = true;

      if (surrounded)
        filled.push_back(pos);

      neighbors.push_back({ pos.first + 1, pos.second });
      neighbors.push_back({ pos.first - 1, pos.second });
      neighbors.push_back({ pos.first, pos.second + 1 });
      neighbors.push_back({ pos.first, pos.second - 1 });
    }

    std::unordered_set<uint32_t> killed{};

    if (surrounded) {
      for (auto& pos : filled) {
        auto [statement, old_owner] = grid[pos.first][pos.second].take(self_id);
        if (statement == TileMap::stmt::DEATH)
          killed.insert(old_owner);
      }
    } else {
      return { {}, {} };
    }

    return { killed, filled };
  }
};

#endif //SPACE_GRID_H