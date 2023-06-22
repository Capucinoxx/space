#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"
#include "grid.h"
#include "structures/concurent_unordered_set.h"
#include "utils.h"

#include <boost/cstdfloat.hpp> 
#include <vector>
#include <unordered_set>
#include <mutex>
#include <stack>
#include <tuple>

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
  using float64_t = boost::float64_t;
  using hsl_color = std::tuple<float64_t, float64_t, float64_t>;

  using position = Grid<ROWS, COLS>::position;

  static constexpr std::size_t NAME_SIZE = 15;

private:
  static constexpr uint32_t MAX_SIZE = (ROWS > COLS) ? ROWS : COLS;

  uint32_t identifier;
  std::string name;
  uint32_t last_frame_played;
  uint32_t frame_alive = 0;
  position current_pos;
  direction last_direction;
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  hsl_color color;
  bool connected;


  ConcurrentUnorderedSet<position, PairHash> trail{ };
  ConcurrentUnorderedSet<position, PairHash> region{ };
  std::mutex mu;

public:
  Player(const std::string& name, uint32_t id, hsl_color color, uint32_t frame, std::shared_ptr<Grid<ROWS, COLS>> grid) 
    : identifier{ id }, name{ name }, last_frame_played{ frame }, grid(std::move(grid)), connected{ true }, color{ color } {
    trail.reserve(ROWS * COLS / 2);
    region.reserve(MAX_SIZE * 2);
    last_direction = direction::DOWN;
  }

  std::string player_name() const noexcept { return name; }
  uint32_t id() const noexcept         { return identifier; }
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

  void set_connection(bool status) noexcept { connected = status; }
  bool is_connected() const noexcept { return connected; }

  double frame_score() const noexcept {
    if (frame_alive == 0) return 0.0;

    double percentage = static_cast<double>(region.size()) / static_cast<double>(ROWS * COLS) * 100.00;
    return percentage * percentage / static_cast<double>(frame_alive);
  }

  void dump_info() {
    std::cout << "Player: " << name << std::endl;
    std::cout << "  - ID: " << identifier << std::endl;
    std::cout << "  - Last frame played: " << last_frame_played << std::endl;
    std::cout << "  - Frame alive: " << frame_alive << std::endl;
    std::cout << "  - Current position: (" << current_pos.first << ", " << current_pos.second << ")" << std::endl;
    std::cout << "  - Last direction: " << last_direction << std::endl;
    std::cout << "  - Trail size: " << trail.size() << std::endl;
    std::cout << "  - Region size: " << region.size() << std::endl;
    std::cout << "  - Score: " << frame_score() << std::endl;
    std::cout << std::endl;
  }

  movement_type perform(uint32_t frame) {
    return perform(frame, last_direction);
  }

  movement_type perform(uint32_t frame, direction d) {
    std::lock_guard<std::mutex> lock(mu);
    if (frame <= last_frame_played)
      return movement_type::IDLE;

    last_frame_played = frame;
    last_direction = d;

    ++frame_alive;

    auto res = move(d);
    if (res == movement_type::IDLE)
      return res;


    if (trail.find(current_pos) != trail.end())
      return movement_type::DEATH;

    if (region.find(current_pos) != region.end())
      return movement_type::COMPLETE;

    res = grid->at(current_pos).step(id());

    trail.insert(current_pos);

    return res;
  }

  void spawn(position p) {
    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto pos = std::make_pair(p.first + i, p.second + j);
        if (is_out_of_bound(pos))
          continue;

        grid->at(pos).step(id());
        region.insert(pos);
      }
    }

    std::lock_guard<std::mutex> lock(mu);
    current_pos = p;
  }

  void fill_region() {
    std::lock_guard<std::mutex> lock(mu);
    if (trail.size() == 0)
      return;

    for (auto& pos : trail) {
      grid->at(pos).step(id());
      region.insert(pos);
    }

    std::array<std::array<bool, COLS>, ROWS> been{};
    std::vector<position> neighbors{};

    neighbors.push_back(current_pos);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_out_of_bound(pos) || been[pos.first][pos.second] || trail.find(pos) != trail.end())
        continue;


      been[pos.first][pos.second] = true;
      region.insert(pos);
      grid->at(pos).take(id());

      flood_fill(std::make_pair(pos.first + 1, pos.second), been);
      flood_fill(std::make_pair(pos.first - 1, pos.second), been);
      flood_fill(std::make_pair(pos.first, pos.second + 1), been);
      flood_fill(std::make_pair(pos.first, pos.second - 1), been);
      
      neighbors.push_back(std::make_pair(pos.first + 1, pos.second));
      neighbors.push_back(std::make_pair(pos.first - 1, pos.second));
      neighbors.push_back(std::make_pair(pos.first, pos.second + 1));
      neighbors.push_back(std::make_pair(pos.first, pos.second - 1));
    }

    trail.clear();
  }

  void flood_fill(position p, std::array<std::array<bool, COLS>, ROWS>& been) {
    if (is_out_of_bound(p) || been[p.first][p.second] || trail.find(p) != trail.end() || grid->at(p).get_value() == id())
      return;

    bool surrounded = true;
    std::vector<position> neighbors{};
    neighbors.reserve(MAX_SIZE);

    std::vector<position> filled{};
    filled.reserve(MAX_SIZE / 2);

    neighbors.push_back(p);
    while (!neighbors.empty()) {
      auto pos = neighbors.back();
      neighbors.pop_back();

      if (is_out_of_bound(pos)) {
        surrounded = false;
        continue;
      }

      if (been[pos.first][pos.second] || trail.find(pos) != trail.end() || grid->at(pos).get_value() == id())
        continue;

      been[pos.first][pos.second] = true;

      if (surrounded)
        filled.push_back(pos);

      neighbors.push_back(std::make_pair(pos.first + 1, pos.second));
      neighbors.push_back(std::make_pair(pos.first - 1, pos.second));
      neighbors.push_back(std::make_pair(pos.first, pos.second + 1));
      neighbors.push_back(std::make_pair(pos.first, pos.second - 1));
    }

    if (surrounded) {
      for (const auto& pos : filled) {
        region.insert(pos);
        grid->at(pos).take(id());
      }
    }
  }

  void death() {  
    for (auto it = trail.begin(); it != trail.end(); ++it)
      grid->at(*it).reset();

    trail.clear();

    for (auto it = region.begin(); it != region.end(); ++it)
      grid->at(*it).reset();

    region.clear();

    std::lock_guard<std::mutex> lock(mu);
    frame_alive = 0;
  }

  void serialize(std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mu);
    serialize_data<std::string>(data, player_name(), NAME_SIZE);
    auto [h, s, l] = color;
    std::cout << "size:" << sizeof(hsl_color) << "value: "<< h << " " << s << " " << l << std::endl;
    serialize_value<hsl_color>(data, color);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint32_t>(data, frame_alive);

    serialize_value<uint32_t>(data, static_cast<uint32_t>(trail.size()));
    for (auto it = trail.begin(); it != trail.end(); ++it) {
      serialize_value<uint32_t>(data, it->first);
      serialize_value<uint32_t>(data, it->second);
    }

    serialize_value<uint32_t>(data, static_cast<uint32_t>(region.size()));
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
      return TileMap::stmt::IDLE;
    }

    current_pos = new_pos;
    return TileMap::stmt::STEP;
  }

  bool is_out_of_bound(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS;
  }
};


#endif //SPACE_PLAYER_H
