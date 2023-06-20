#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include "player.h"
#include "grid.h"
#include "utils.h"
#include "structures/concurrent_unordered_map.h"

#include <iostream> 
#include <thread>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <random>

template<uint32_t ROWS, uint32_t COLS>
class Spawn {
private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;

  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> dist_x;
  std::uniform_int_distribution<uint32_t> dist_y;

public:
  Spawn(std::shared_ptr<Grid<ROWS, COLS>> grid) : grid(std::move(grid)), gen{ rd() } { 
    dist_x = std::uniform_int_distribution<uint32_t>(0, ROWS - 1);
    dist_y = std::uniform_int_distribution<uint32_t>(0, COLS - 1);
  }

  std::pair<uint32_t, uint32_t> operator()() {
    uint32_t x = dist_x(gen);
    uint32_t y = dist_y(gen);

    int retry = 0;

    while (position_is_valid({ x, y }) && retry++ < 10) {
      x = dist_x(gen);
      y = dist_y(gen);
    }

    return { x, y };
  }

  bool position_is_valid(const std::pair<uint32_t, uint32_t>& pos) {
    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto px = pos.first + i;
        auto py = pos.second + j;

        bool is_out_of_bounds = px >= ROWS || py >= COLS;
        if (is_out_of_bounds || grid->at({ px, py }).is_trail())
          return true;
      }
    }

    

    return false;
  }
};

template<uint32_t ROWS, uint32_t COLS>
class GameManager {
public:
  using position = Player<ROWS, COLS>::position;
  using broadcast_fn_type = void (*)(const std::vector<uint8_t>&);

private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

  ConcurrentUnorderedMap<uint32_t, std::shared_ptr<Player<ROWS, COLS>>> players{ };
  Spawn<ROWS, COLS> spawn;
  
  std::thread th;
  uint32_t frame_count = 0;
  std::atomic<bool> running{ false };

  UniqueIDGenerator<15> uuid_generator;

public:
  explicit GameManager() : grid(std::make_shared<Grid<ROWS, COLS>>()), spawn(grid) {}

  ~GameManager() = default;
  GameManager(const GameManager&) = delete;
  GameManager& operator=(const GameManager&) = delete;

  std::shared_ptr<Player<ROWS, COLS>> register_player(const std::string& name, uint32_t id) {
    auto it = players.find(id);
    if (it != players.end()) {
      if (it->second->is_connected())
        return nullptr;

      it->second->set_connection(true);
      return it->second;
    }

    auto p = std::make_shared<Player<ROWS, COLS>>(name, id, frame_count, grid);
    spawn_player(p);
    players.insert(id, p);

    return p;
  }

  bool is_running() const noexcept { return running.load(); }
  uint32_t frame() const noexcept { return frame_count; }

  void remove_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    auto it = players.find(p->id());
    if (it != players.end())
      it->second->set_connection(false);
  }

  std::string generate_secret() { return uuid_generator(); }

  template<typename T>
  void start(T* object, void (T::*callback)(const std::vector<uint8_t>&)) { 
    if (running.load())
      return;

    running.store(true);
    th = std::thread([object, callback, this]() {
      while(running) {
        update_map();
        auto data = serialize();
        (object->*callback)(data);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });
  }
  
  void stop()  { 
    running.store(false);
    if (th.joinable())
      th.join();
  }
  
  std::shared_ptr<Grid<ROWS, COLS>> get_grid() const noexcept {
    return grid;
  }

  void spawn_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    // auto position = std::make_pair(uint32_t(20), uint32_t(20));

    p->spawn(spawn());
  }

  void handle_move_result(std::shared_ptr<Player<ROWS, COLS>> p, Player<ROWS, COLS>::movement_type movement) {
    switch (movement) {
      case Player<ROWS, COLS>::movement_type::DEATH: {
        auto it = players.find(grid->at(p->pos()).get_value());
        if (it != players.end()) {
          auto victim = it->second;
          std::cout << "--- KILL by " << p->id() << " ---" << std::endl; 
          victim->dump_info();
          victim->death();

          spawn_player(victim);

          if (victim->id() != p->id())
            std::cout << "is not the same" << std::endl;
        }
        break;
      }
      case Player<ROWS, COLS>::movement_type::COMPLETE:
        p->fill_region();
        break;

      default:
        break;
    }
  }

  std::vector<uint8_t> serialize() {
    std::vector<uint8_t> data;

    serialize_value<uint32_t>(data, ROWS);
    serialize_value<uint32_t>(data, COLS);
    serialize_value<uint32_t>(data, frame());

    for (const auto& player : players)
      player.second->serialize(data);

    return data;
  }

private:
  void update_map() {
    for (auto& p : players)
      p.second->perform(frame_count);
    ++frame_count;
  }
};

#endif //SPACE_MAP_H
