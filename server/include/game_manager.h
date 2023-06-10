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
public:
  using position = Player<ROWS, COLS>::position;

private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  std::vector<std::shared_ptr<Player<ROWS, COLS>>> players;
  std::vector<std::pair<uint32_t, uint32_t>> spawns;
  std::atomic<int> current_spawn { 1 };
  std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

  std::unique_ptr<std::thread> th;
  uint32_t frame_count = 0;

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

  uint32_t frame() const noexcept {
    return frame_count;
  }

  std::shared_ptr<Grid<ROWS, COLS>> get_grid() const noexcept {
    return grid;
  }

  void stop() {
    if (th->joinable())
      th->join();
  }

  void spawn_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    // auto position = spawns[current_spawn++ % spawns.size()];

    auto position = std::make_pair(uint32_t(20), uint32_t(20));

    p->spawn(position);
  }

  void handle_move_result(std::shared_ptr<Player<ROWS, COLS>> p, Player<ROWS, COLS>::movement_type movement) {
    switch (movement) {
      case Player<ROWS, COLS>::movement_type::DEATH:
        p->death();
        spawn_player(p);
        break;
      case Player<ROWS, COLS>::movement_type::COMPLETE:
        p->fill_region();
        break;
    }
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
    serialize_value<uint32_t>(data, frame());

    for (const auto& player : players)
      player->serialize(data);

    return data;
  }

private:
  void update_map() {
    for (auto& p : players)
      p->perform(frame_count);
    ++frame_count;
  }

  void generate_spawns() {
    uint32_t num_spawns = std::sqrt(ROWS * COLS);
    double min_distance = std::sqrt(static_cast<double>(ROWS * COLS)) / num_spawns;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<std::vector<bool>> spawn_grid(ROWS, std::vector<bool>(COLS, false));
    std::vector<position> active_spawns;

    auto is_valid_point = [&](position p) {
      if (p.first < COLS && p.second < ROWS)
        return !spawn_grid[p.second][p.first];
      return false;
    };

    auto add_point = [&](position p) {
      active_spawns.push_back(p);
      spawn_grid[p.second][p.first] = true;
      spawns.push_back(p);
    };

    position start = {  static_cast<int>(distribution(rng) * COLS), 
                        static_cast<int>(distribution(rng) * ROWS) };
    add_point(start);
    

    while (!active_spawns.empty()) {
      std::uniform_int_distribution<uint32_t> distribution(0, active_spawns.size() - 1);
      uint32_t index = distribution(rng);
      position active_pos = active_spawns[index];
      active_spawns.erase(active_spawns.begin() + index);

      const int num_attemps = 30;
      for (int i = 0; i != num_attemps; ++i) {
        double angle = 2.0 * M_PI * distribution(rng);
        double distance = min_distance * (1.0 + distribution(rng));
        uint32_t x = static_cast<uint32_t>(distance * std::cos(angle));
        uint32_t y = static_cast<uint32_t>(distance * std::sin(angle));
        
        position new_pos = { active_pos.first + x, active_pos.second + y };
        if (is_valid_point(new_pos)) {
          add_point(new_pos);
          active_spawns.push_back(new_pos);
        }
      }
    }
  }
};

#endif //SPACE_MAP_H
