#ifndef SPACE_GRID_H
#define SPACE_GRID_H

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "tile_map.h"
#include "player.h"
#include "utils.h"

template<uint32_t ROWS, uint32_t COLS>
class Grid {
public:
  using position = std::pair<uint32_t, uint32_t>;
  using player_ptr = std::shared_ptr<Player<ROWS, COLS>>;

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

  std::unordered_map<uint32_t, std::unordered_set<position, PairHash>> fill_region(player_ptr p) {
    if (p->trail().empty())
      return {};

    std::unordered_set<std::pair<uint32_t, uint32_t>, PairHash> trail{};
    std::unordered_map<uint32_t, std::unordered_set<position, PairHash>> tiles_to_investigate{};

    auto insert_tiles = [&](uint32_t owner, const position& pos) {
      if (tiles_to_investigate.find(owner) == tiles_to_investigate.end())
        tiles_to_investigate[owner] = std::unordered_set<position, PairHash>{};

      tiles_to_investigate[owner].insert(pos);
    };


    p->for_each_trail([&](const position& pos) {
      at(pos).take(p->id());
      trail.insert(pos);
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

      if (is_invalid_pos(pos) || been[pos.first][pos.second] || trail.find(pos) != trail.end())
        continue;

      been[pos.first][pos.second] = true;

      p->append_region({ pos });
      auto [statement, old_owner] = at(pos).take(p->id());
      if (old_owner != 0 && old_owner != p->id())
        insert_tiles(old_owner, pos);

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

        for (const auto& pos : res) {
          auto [statement, old_owner] = at(pos).take(p->id());
          if (old_owner != 0 && old_owner != p->id())
            insert_tiles(old_owner, pos);
        }

        p->append_region(res);
        neighbors.push_back({ px, py });
      }
    }

    return tiles_to_investigate;
  }

  bool is_invalid_pos(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS;
  }

private:
  std::vector<position> flood_fill(uint32_t self_id, const position& pos, std::array<std::array<bool, COLS>, ROWS>& been) {
    if (is_invalid_pos(pos) || been[pos.first][pos.second])
      return {};

    std::vector<position> neighbors{};
    neighbors.reserve(ROWS > COLS ? ROWS : COLS);

    std::vector<position> filled{};
    filled.reserve(ROWS > COLS ? ROWS : COLS);

    neighbors.push_back(pos);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_invalid_pos(pos))
        return {};

      if (been[pos.first][pos.second] || at(pos).owner() == self_id)
        continue;

      been[pos.first][pos.second] = true;
      filled.push_back(pos);

      neighbors.push_back({ pos.first + 1, pos.second });
      neighbors.push_back({ pos.first - 1, pos.second });
      neighbors.push_back({ pos.first, pos.second + 1 });
      neighbors.push_back({ pos.first, pos.second - 1 });
    }

    return { filled };
  }
};

#endif //SPACE_GRID_H