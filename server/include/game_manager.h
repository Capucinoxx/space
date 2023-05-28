#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <functional>
#include <memory>

#include "player.h"
#include "tile_map.h"
#include "utils.h"

template<uint32_t ROWS, uint32_t COLS>
class GameManager {
public:
  using broadcast_fn = std::function<void(std::vector<uint8_t>&)>;

private:
  std::array<std::array<TileMap, COLS>, ROWS> grid;
  std::vector<std::shared_ptr<Player<ROWS, COLS>>> players;
  std::vector<std::pair<uint32_t, uint32_t>> spawns;
  std::atomic<int> current_spawn { 1 };

  std::thread th;
  std::atomic<bool> running { true };
  uint32_t tick;
  broadcast_fn broadcast;

public:
  GameManager(uint32_t tick, broadcast_fn broadcast) : tick{ tick }, broadcast(broadcast) {
    generate_spawns();
  }

  void register_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    players[p->id()] = p;
  }

  void start() {
    th = std::thread([this]() {
      while (running) {
        update_map();

        broadcast_map();

        std::this_thread::sleep_for(std::chrono::milliseconds(tick));
      }
    });
  }

  void stop() {
    running.store(false);
    if (th.joinable())
      th.join();
  }

private:
  void update_map() {}

  void broadcast_map() {
    // todo generate map
    std::vector<uint8_t> message = serialize();

    broadcast(message);
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

  std::vector<uint8_t> serialize() {
    std::vector<uint8_t> data;

    serialize_value<uint32_t>(data, ROWS);
    serialize_value<uint32_t>(data, COLS);

    // players
    for (const auto& player : players)
      player->serialize(data);

    return data;
  }


};

//
//template<std::size_t ROWS, std::size_t COLS>
//class game_manager {
//public:
//  using player_position = typename player<ROWS, COLS>::position_type;
//
//  static const int MAX_PLAYERS = 40;
//
//private:
//  tile_map fields[ROWS][COLS];
//  std::vector<player<ROWS, COLS> *> players;
//  std::vector<player_position> spawns;
//  std::size_t current_spawn = 0;
//
//public:
//  game_manager() { generate_spawns(); }
//  void update() {
//    for (const player<ROWS, COLS>& p : players) {
//      tile_map::stmt res = p.update(fields);
//      if (res == tile_map::stmt::DEATH)
//        kill(p);
//      else if(res == tile_map::stmt::COMPLETE)
//        complete_area(p);
//    }
//  }
//
//  void spawn(player<ROWS, COLS> *p) {
//    auto [x_pos, y_pos] = spawns[current_spawn++];
//    current_spawn %= spawns.size();
//
//    for (int i = x_pos - 1; i != x_pos + 2; ++i)
//      for (int j = x_pos - 1; j != y_pos + 2; ++j)
//        fields[i][j].set(p->id());
//
//    p->set_position(std::make_pair(x_pos, y_pos));
//  }
//
//  void kill(player<ROWS, COLS> *p) {
//    player_position pos = p->position();
//
//    for (auto it = p->trail_begin(); it != p->trail_end(); ++it) {
//      const auto& trail_point = *it;
//
//      if (trail_point == pos)
//        continue;
//
//      auto [x, y] = pos;
//
//      if (fields[x][y].is_step()) {
//        fields[x][y].reset();
//      }
//    }
//  }
//
//  bool complete_area(player<ROWS, COLS> *p) {
//    return false;
//  }
//
//  bool is_in_bounds(player_position pos) {
//    return false;
//  }
//
//  std::string serialize() const {
//    return std::string();
//  }
//
//private:
//  void generate_spawns() {
//    int n_spawns = MAX_PLAYERS * 2;
//
//    int n_cols = static_cast<int>(std::sqrt(n_spawns));
//    int n_rows = static_cast<int>((n_spawns + n_cols - 1) / n_cols);
//    float spacing_x = static_cast<float>(COLS) / n_cols;
//    float spacing_y = static_cast<float>(ROWS) / n_rows;
//
//    std::random_device rd;
//    std::mt19937 rng(rd());
//    std::uniform_real_distribution<float> dist(-0.2f, 0.2f);
//
//    for (int i = 0; i != n_rows; ++i)
//      for (int j = 0; j != n_cols; ++j) {
//        float offset_x = dist(rng) * spacing_x;
//        float offset_y = dist(rng) * spacing_y;
//
//        int x = static_cast<int>(j * spacing_x + spacing_x / 2 + offset_x);
//        int y = static_cast<int>(i * spacing_y + spacing_y / 2 + offset_y);
//
//        spawns.push_back({x, y});
//        std::shuffle(spawns.begin(), spawns.end(), rng);
//      }
//  }
//
//  class serializer {
//  public:
//    std::string operator()( const std::vector<player<ROWS, COLS> *>& players,
//                            const std::array<std::array<uint8_t, COLS>, ROWS>& grid) const {
//      std::stringstream ss;
//
//      // serialize dimensions
//      append(ss, static_cast<int32_t>(ROWS));
//      append(ss, static_cast<int32_t>(COLS));
//
//      // Serialize map data
//      ss.write(reinterpret_cast<const char *>(grid.data()), ROWS * COLS * sizeof(uint8_t));
//
//      // Serialize number of players
//      append(ss, static_cast<uint8_t>(players.size()));
//
//      // serialize player data
//      for (const auto& p : players) {
//        append(ss, p->id());
//        append(ss, static_cast<uint8_t>(p->name().size()));
//        ss.write(p->name().c_str(), p->name().size());
//      }
//
//      return ss.str();
//    }
//
//  private:
//    template<typename T>
//    void append(std::stringstream& ss, const T& value) const {
//      ss.write(reinterpret_cast<const char *>(&value), sizeof(value));
//    }
//  };
//};

#endif //SPACE_MAP_H
