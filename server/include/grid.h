#ifndef SPACE_GRID_H
#define SPACE_GRID_H

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "tile_map.h"
#include "common.h"
#include "player/player.h"

template<uint32_t ROWS, uint32_t COLS>
class Grid {
public:
  using position = std::pair<uint32_t, uint32_t>;
  using player_ptr = std::shared_ptr<player>;

private:
  std::array<std::array<TileMap, COLS>, ROWS> grid;

public:
  Grid() = default;

  void clear() {
    for (auto& row : grid)
      for (auto& tile : row)
        tile.clear();
  }

  TileMap& at(const position& pos) {
    return grid[pos.first][pos.second];
  }

  std::unordered_map<uint32_t, std::unordered_set<position, pair_hash>> fill_region(player_ptr p) {
    if (p->get_trail().empty())
      return {};

    std::unordered_map<uint32_t, std::unordered_set<position, pair_hash>> tiles_to_investigate{};
    std::unordered_set<position, pair_hash> trail{};

    auto insert_tiles = [&](const position& pos) {
      auto [statement, old_owner] = at(pos).take(p->id());
      if (old_owner == 0 || old_owner == p->id())
        return;

      if (tiles_to_investigate.find(old_owner) == tiles_to_investigate.end())
        tiles_to_investigate[old_owner] = std::unordered_set<position, pair_hash>{};

      tiles_to_investigate[old_owner].insert(pos);
    };

    p->for_each_trail([&](const position& pos) {
      trail.insert(pos);
    });

    std::cout << std::endl;

    std::array<std::array<bool, COLS>, ROWS> been{};
    std::vector<position> neighbors{};

    neighbors.reserve(ROWS > COLS ? ROWS : COLS);
    neighbors.push_back(p->pos());

    auto is_first = true;

    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_invalid_pos(pos) || been[pos.first][pos.second])
        continue;

      been[pos.first][pos.second] = true;

      auto movements = {
        std::make_pair(1, 0),
        std::make_pair(-1, 0),
        std::make_pair(0, 1),
        std::make_pair(0, -1)
      };

      if (trail.find(pos) == trail.end() && !is_first)
        continue;

      is_first = false;

      been[pos.first][pos.second] = true;
      at(pos).take(p->id());
      p->append_region({ pos });

      for (auto& movement : movements) {
        auto px = pos.first + movement.first;
        auto py = pos.second + movement.second;

        auto res = flood_fill(p->id(), { px, py }, trail, been);

        for (const auto& position : res) {
          insert_tiles(position);
        }

        p->append_region(res);
        neighbors.emplace_back( px, py );
      }
    }

    return tiles_to_investigate;
  }

  [[nodiscard]] bool is_invalid_pos(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS;
  }

private:
  std::vector<position> flood_fill(uint32_t self_id, const position& start_pos, const std::unordered_set<position, pair_hash>& trail, std::array<std::array<bool, COLS>, ROWS>& been) {
    if (is_invalid_pos(start_pos) || been[start_pos.first][start_pos.second] || trail.find(start_pos) != trail.end())
      return {};

    std::vector<position> neighbors{};
    neighbors.reserve(ROWS > COLS ? ROWS : COLS);

    std::vector<position> filled{};
    filled.reserve(ROWS > COLS ? ROWS : COLS);

    bool surrounded = true;

    neighbors.push_back(start_pos);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_invalid_pos(pos)) {
        surrounded = false;
        continue;
      }

      if (been[pos.first][pos.second] || at(pos).owner() == self_id || trail.find(pos) != trail.end())
        continue;

      been[pos.first][pos.second] = true;

      if (surrounded)
        filled.push_back(pos);

      neighbors.emplace_back( pos.first + 1, pos.second );
      neighbors.emplace_back( pos.first - 1, pos.second );
      neighbors.emplace_back( pos.first, pos.second + 1 );
      neighbors.emplace_back( pos.first, pos.second - 1 );
    }

    if (surrounded)
      return { filled };

    return {};
  }
};

#endif //SPACE_GRID_H