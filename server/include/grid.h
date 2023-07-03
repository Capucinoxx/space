#ifndef SPACE_GRID_H
#define SPACE_GRID_H

#include <memory>
#include <unordered_set>

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

  std::unordered_set<std::pair<uint32_t, uint32_t>, PairHash> inacessible_areas;

public:
  Grid() = default;

  TileMap& at(const position& pos) {
    return grid[pos.first][pos.second];
  }

  void serialize(std::vector<uint8_t>& buffer) {
    serialize_value<uint32_t>(buffer, inacessible_areas.size());
    for (auto& inacessible_tile : inacessible_areas) {
      serialize_value<uint32_t>(buffer, inacessible_tile.first);
      serialize_value<uint32_t>(buffer, inacessible_tile.second);
    }
  }

  std::unordered_set<uint32_t> fill_region(player_ptr p) {
    std::unordered_set<uint32_t> killed{};
    std::unordered_set<std::pair<uint32_t, uint32_t>, PairHash> trail{};

    std::size_t tile_coverage = 0;

    p->for_each_trail([&](const position& pos) {
      at(pos).take(p->id());
      ++tile_coverage;
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

      ++tile_coverage;
      p->append_region({ pos });
      auto [statement, old_owner] = at(pos).take(p->id());
      if (statement == TileMap::stmt::DEATH && old_owner != p->id())
        killed.insert(old_owner);

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
        ++++tile_coverage;

        for (const auto& pos : res.second) {
          auto [statement, old_owner] = at(pos).take(p->id());
          if (statement == TileMap::stmt::DEATH && old_owner != p->id())
            killed.insert(old_owner);
        }

        p->append_region(res.second);
        killed.insert(res.first.begin(), res.first.end());

        neighbors.push_back({ px, py });
      }
    }

    return killed;
  }

  bool is_invalid_pos(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS || inacessible_areas.find(pos) != inacessible_areas.end();
  }

private:
  std::pair<std::unordered_set<uint32_t>, std::vector<position>> flood_fill(uint32_t self_id, const position& pos, std::array<std::array<bool, COLS>, ROWS>& been) {
    if (is_invalid_pos(pos) || been[pos.first][pos.second])
      return { {}, {} };

    std::vector<position> neighbors{};
    neighbors.reserve(ROWS > COLS ? ROWS : COLS);

    std::vector<position> filled{};
    filled.reserve(ROWS > COLS ? ROWS : COLS);

    neighbors.push_back(pos);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_invalid_pos(pos))
        return { {}, {} };

      if (been[pos.first][pos.second] || at(pos).owner() == self_id)
        continue;

      been[pos.first][pos.second] = true;
      filled.push_back(pos);

      neighbors.push_back({ pos.first + 1, pos.second });
      neighbors.push_back({ pos.first - 1, pos.second });
      neighbors.push_back({ pos.first, pos.second + 1 });
      neighbors.push_back({ pos.first, pos.second - 1 });
    }

    std::unordered_set<uint32_t> killed{};

    for (auto& pos : filled) {
      auto [statement, old_owner] = grid[pos.first][pos.second].take(self_id);
      if (statement == TileMap::stmt::DEATH)
        killed.insert(old_owner);
    }

    return { killed, filled };
  }
};

#endif //SPACE_GRID_H