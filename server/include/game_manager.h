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


template<uint32_t ROWS, uint32_t COLS>
class GameManager {
public:
  using position = Player<ROWS, COLS>::position;
  using broadcast_fn_type = void (*)(const std::vector<uint8_t>&);

private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

  ConcurrentUnorderedMap<uint32_t, std::shared_ptr<Player<ROWS, COLS>>> players{ };
  
  std::thread th;
  uint32_t frame_count = 0;
  std::atomic<bool> running{ false };

  UniqueIDGenerator<15> uuid_generator;

public:
  explicit GameManager() { 
    grid = std::make_shared<Grid<ROWS, COLS>>();
  }

  ~GameManager() = default;
  GameManager(const GameManager&) = delete;
  GameManager& operator=(const GameManager&) = delete;

  bool register_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    if (players.contains(p->id()))
      return false;

    spawn_player(p);

    players.insert(p->id(), p);
    return true;
  }

  bool is_running() const noexcept { return running.load(); }
  uint32_t frame() const noexcept { return frame_count; }

  void remove_player(std::shared_ptr<Player<ROWS, COLS>> p) {
    players.erase(p->id());
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
    auto position = std::make_pair(uint32_t(20), uint32_t(20));

    p->spawn(position);
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
