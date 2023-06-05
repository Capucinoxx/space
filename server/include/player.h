#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"

#include "utils.h"

#include <vector>
#include <unordered_set>
#include <mutex>

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

template<uint32_t ROWS, uint32_t COLS>
class Player {
public:
  enum direction { UP, DOWN, LEFT, RIGHT };
  using movement_type = TileMap::stmt;

  using position = std::pair<uint32_t, uint32_t>;
  using trail_iterator = typename std::vector<position>::iterator;
  using const_trail_iterator = typename std::vector<position>::const_iterator;

  static const int UUID_SIZE = 15;

private:
  static constexpr uint32_t MAX_SIZE = (ROWS > COLS) ? ROWS : COLS;

  std::string uuid;
  uint8_t game_manager_idx = 1; // todo use game_manager idx
  direction next_direction;
  position current_pos;
  position next_position;
  std::vector<position> trail;

  std::unordered_set<std::pair<uint32_t, uint32_t>, PairHash> region;
  std::mutex region_mutex;

  uint32_t frame_alive = 0;


public:
  Player(const std::string& uuid) : uuid{ uuid } {
    // temporaire
    next_direction = DOWN;
    trail.reserve(MAX_SIZE);
    region.reserve(MAX_SIZE * 2);
  }

  static direction parse_action(const std::string& data) {
    switch (static_cast<uint8_t>(data[0])) {
      case 0: return UP;
      case 1: return DOWN;
      case 2: return LEFT;
      case 3: return RIGHT;
      default: return LEFT;
    }
  }

  void set_direction(direction d) noexcept {
    next_direction = d;
  }

  void append_region(position p) noexcept {
    std::lock_guard<std::mutex> lock(region_mutex);
    region.insert(p);
  }


  void set_position(position p) noexcept {
    current_pos = p;
  }

  uint8_t idx() const noexcept { return game_manager_idx; }

  std::string id() const noexcept { return uuid; }
  position pos() const noexcept   { return current_pos; }

  trail_iterator begin() noexcept { return trail.begin(); }
  trail_iterator end() noexcept   { return trail.end(); }

  const_trail_iterator begin() const noexcept { return trail.begin(); }
  const_trail_iterator end() const noexcept   { return trail.end(); }

  movement_type update() {
    ++frame_alive;

    auto res = move(next_direction);

    if (region.find(current_pos) != region.end())
      return movement_type::IDLE;


    if (res == movement_type::IDLE)
      return res;

    trail.push_back(current_pos);

    return res;
  }

  void serialize(std::vector<uint8_t>& data) {
    serialize_data<std::string>(data, id(), UUID_SIZE);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint32_t>(data, frame_alive);

    // serialize trail
    serialize_value<uint8_t>(data, static_cast<uint8_t>(trail.size()));
    for (auto it = begin(); it != end(); ++it) {
      serialize_value<uint32_t>(data, it->first);
      serialize_value<uint32_t>(data, it->second);
    }

    // serialize region
    std::lock_guard<std::mutex> lock(region_mutex);
    {
      serialize_value<uint32_t>(data, static_cast<uint8_t>(region.size()));
      for (auto it = region.begin(); it != region.end(); ++it) {
        serialize_value<uint32_t>(data, it->first);
        serialize_value<uint32_t>(data, it->second);
      }
    }
  }

private:
  movement_type move(direction d) noexcept {
    // todo: if out of bound, return IDLE
    switch (d) {
      case UP:    
        --current_pos.second; 
        if (is_out_of_bound()) {
          ++current_pos.second;
          return movement_type::IDLE;
        }
        break;

      case DOWN:  
        ++current_pos.second; 
        if (is_out_of_bound()) {
          --current_pos.second;
          return movement_type::IDLE;
        }
        break;

      case LEFT:  
        --current_pos.first;
        if (is_out_of_bound()) {
          ++current_pos.first;
          return movement_type::IDLE;
        }
        break;

      case RIGHT:
        ++current_pos.first;
        if (is_out_of_bound()) {
          --current_pos.first;
          return movement_type::IDLE;
        }
        break;
    }

    // todo: on aurait besoin de la map pour savoir le type de mouvement fait
    return movement_type::STEP;
  }

  bool is_out_of_bound() const noexcept {
    return current_pos.first >= ROWS || current_pos.second >= COLS;
  }
};

#endif //SPACE_PLAYER_H
