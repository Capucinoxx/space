#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"
#include "grid.h"
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

  using position = Grid<ROWS, COLS>::position;

  static constexpr std::size_t NAME_SIZE = 15;

private:
  static constexpr uint32_t MAX_SIZE = (ROWS > COLS) ? ROWS : COLS;

  std::string name;
  uint8_t game_idx = 1;
  uint32_t last_frame_played;
  uint32_t frame_alive = 0;
  position current_pos;
  direction last_direction;;
  std::shared_ptr<Grid<ROWS, COLS>> grid;

  std::unordered_set<position, PairHash> trail;
  std::unordered_set<position, PairHash> region;
  std::mutex region_mu;

public:
  Player(const std::string& name, uint32_t frame, std::shared_ptr<Grid<ROWS, COLS>> grid) 
    : name{ name }, last_frame_played{ frame }, grid(std::move(grid)) {
    trail.reserve(ROWS * COLS / 2);
    region.reserve(MAX_SIZE * 2);
    last_direction = direction::DOWN;
  }

  std::string player_name() const noexcept { return name; }
  uint8_t game_id() const noexcept         { return game_idx; }
  position pos() const noexcept            { return current_pos; }

  static direction const parse_action(const std::string& data) {
    switch (static_cast<uint8_t>(data[0])) {
      case 0: return direction::UP;
      case 1: return direction::DOWN;
      case 2: return direction::LEFT;
      case 3: return direction::RIGHT;
      default: return direction::DOWN;
    }
  }

  double frame_score() const noexcept {
    if (frame_alive == 0) return 0.0;

    double percentage = static_cast<double>(region.size()) / static_cast<double>(ROWS * COLS) * 100.00;
    return percentage * percentage / static_cast<double>(frame_alive);
  }

  movement_type perform(uint32_t frame) {
    return perform(frame, last_direction);
  }

  movement_type perform(uint32_t frame, direction d) {
    if (frame <= last_frame_played)
      return movement_type::IDLE;

    last_frame_played = frame;

    ++frame_alive;

    auto res = move(d);

    if (trail.find(current_pos) != trail.end())
      return movement_type::DEATH;

    if (region.find(current_pos) != region.end())
      return movement_type::COMPLETE;

    if (res == movement_type::IDLE)
      return res;



    trail.insert(current_pos);



    return res;
  }

  void spawn(position p) {
    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto pos = std::make_pair(p.first + i, p.second + j);
        if (is_out_of_bound(pos))
          continue;

        grid->at(pos).step(game_id());
        region.insert(pos);
      }
    }

    current_pos = p;
  }

  void death() {  
    for (auto it = trail.begin(); it != trail.end(); ++it)
      grid->at(*it).reset();

    trail.clear();

    for (auto it = region.begin(); it != region.end(); ++it)
      grid->at(*it).reset();

    region.clear();

    frame_alive = 0;
  }

  void serialize(std::vector<uint8_t>& data) {
    serialize_data<std::string>(data, player_name(), NAME_SIZE);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint32_t>(data, frame_alive);

    serialize_value<uint8_t>(data, static_cast<uint8_t>(trail.size()));
    for (auto it = trail.begin(); it != trail.end(); ++it) {
      serialize_value<uint32_t>(data, it->first);
      serialize_value<uint32_t>(data, it->second);
    }

    serialize_value<uint32_t>(data, static_cast<uint8_t>(region.size()));
    for (auto it = region.begin(); it != region.end(); ++it) {
      serialize_value<uint32_t>(data, it->first);
      serialize_value<uint32_t>(data, it->second);
    }
  }

private:
  movement_type move(direction d) noexcept {
    auto new_pos = current_pos;

    switch (d) {
      case UP:    --new_pos.second; break;
      case DOWN:  ++new_pos.second; break;
      case LEFT:  --new_pos.first; break;
      case RIGHT: ++new_pos.first; break;
    }

    if (is_out_of_bound(new_pos)) {
      std::cout << "OUT OF BOUND" << std::endl;
      return TileMap::stmt::IDLE;
    }

    current_pos = new_pos;
    return TileMap::stmt::STEP;
  }

  bool is_out_of_bound(const position& pos) const noexcept {
    return pos.first > ROWS || pos.second > COLS;
  }
};


#endif //SPACE_PLAYER_H
