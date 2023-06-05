#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include "player.h"
#include "grid.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <thread>
#include <unordered_map>
#include <functional>
#include <memory>

template<uint32_t ROWS, uint32_t COLS>
class GameManager {
private:
  // std::array<std::array<TileMap, COLS>, ROWS> grid;
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  std::vector<std::shared_ptr<Player<ROWS, COLS>>> players;
  std::vector<std::pair<uint32_t, uint32_t>> spawns;
  std::atomic<int> current_spawn { 1 };
  std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

  std::unique_ptr<std::thread> th;
  uint32_t frame = 0;

public:
  GameManager() { 
    generate_spawns();
    grid = std::make_shared<Grid<ROWS, COLS>>();
  }

  ~GameManager() = default;
  GameManager(const GameManager&) = delete;
  GameManager& operator=(const GameManager&) = delete;

  void register_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    spawn_player(p);
    players.push_back(std::move(p));
  }

  void update() {
    update_map();
  }

  void stop() {
    if (th->joinable())
      th->join();
  }

  void spawn_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    auto position = spawns[current_spawn++ % spawns.size()];

    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto px = position.first + i;
        auto py = position.second + j;
        grid->at({ px, py }).step(p->idx());

        // (*grid)[{ px, py }]->step(p->idx());
        p->append_region({ px, py });
      }
    }

    p->set_position(position);
  }

  void kill() {
    // TODO
  }

  void complete_area() {
    // TODO
  }

  bool is_out_of_bounds() {
    return false;
  }

  std::vector<uint8_t> serialize() {
    std::vector<uint8_t> data;

    serialize_value<uint32_t>(data, ROWS);
    serialize_value<uint32_t>(data, COLS);
    serialize_value<uint32_t>(data, frame);

    for (const auto& player : players)
      player->serialize(data);

    return data;
  }

private:
  void update_map() {
    for (auto& p : players)
      p->update();
    ++frame;
  }

  void generate_spawns() {
    int n_spawns = static_cast<int>(std::sqrt(ROWS * COLS));

    int n_cols = static_cast<int>(std::sqrt(n_spawns));
    int n_rows = static_cast<int>((n_spawns + n_cols - 1) / n_cols);

    float spacing_x = static_cast<float>(COLS) / n_cols;
    float spacing_y = static_cast<float>(ROWS) / n_rows;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> dist(-0.2f, 0.2f);

    for (int i = 0; i != n_rows; ++i) {
      for (int j = 0; j != n_cols; ++j) {
        float offset_x = dist(rng) * spacing_x;
        float offset_y = dist(rng) * spacing_y;

        int x = static_cast<int>(i * spacing_x + spacing_x / 2 + offset_x);
        int y = static_cast<int>(j * spacing_y + spacing_y / 2 + offset_y);

        spawns.push_back({ x, y });
      }
    }

    std::shuffle(spawns.begin(), spawns.end(), rng);
  }
};

#endif //SPACE_MAP_H
