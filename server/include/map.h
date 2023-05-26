#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

#include "player.h"
#include "tile_map.h"

template<std::size_t ROWS, std::size_t COLS>
class map {
public:
  using player_position = typename player<ROWS, COLS>::position_type;

  static const int MAX_PLAYERS = 40;

private:
  tile_map fields[ROWS][COLS];
  std::vector<player<ROWS, COLS> *> players;
  std::vector<player_position> spawns;
  std::size_t current_spawn = 0;

public:
  map() { generate_spawns(); }
  void update() {
    for (const player<ROWS, COLS>& p : players) {
      tile_map::stmt res = p.update(fields);
      if (res == tile_map::stmt::DEATH)
        kill(p);
      else if(res == tile_map::stmt::COMPLETE)
        complete_area(p);
    }
  }

  void spawn(player<ROWS, COLS> *p) {
    auto [x_pos, y_pos] = spawns[current_spawn++];
    current_spawn %= spawns.size();

    for (int i = x_pos - 1; i != x_pos + 2; ++i)
      for (int j = x_pos - 1; j != y_pos + 2; ++j)
        fields[i][j].set(p->id());

    p->set_position(std::make_pair(x_pos, y_pos));
  }

  void kill(player<ROWS, COLS> *p) {
    player_position pos = p->position();

    for (auto it = p->trail_begin(); it != p->trail_end(); ++it) {
      const auto& trail_point = *it;

      if (trail_point == pos)
        continue;

      auto [x, y] = pos;

      if (fields[x][y].is_step()) {
        fields[x][y].reset();
      }
    }
  }

  bool complete_area(player<ROWS, COLS> *p) {
    return false;
  }

  bool is_in_bounds(player_position pos) {
    return false;
  }

private:
  void generate_spawns() {
    int n_spawns = MAX_PLAYERS * 2;

    int n_cols = static_cast<int>(std::sqrt(n_spawns));
    int n_rows = static_cast<int>((n_spawns + n_cols - 1) / n_cols);
    float spacing_x = static_cast<float>(COLS) / n_cols;
    float spacing_y = static_cast<float>(ROWS) / n_rows;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> dist(-0.2f, 0.2f);

    for (int i = 0; i != n_rows; ++i)
      for (int j = 0; j != n_cols; ++j) {
        float offset_x = dist(rng) * spacing_x;
        float offset_y = dist(rng) * spacing_y;

        int x = static_cast<int>(j * spacing_x + spacing_x / 2 + offset_x);
        int y = static_cast<int>(i * spacing_y + spacing_y / 2 + offset_y);

        spawns.push_back(x, y);
        std::shuffle(spawns.begin(), spawns.end(), rng);
      }
  }
};

#endif //SPACE_MAP_H
