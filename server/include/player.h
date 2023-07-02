#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"
#include "structures/concurent_unordered_set.h"
#include "utils.h"
#include "action.h"

#include <boost/cstdfloat.hpp> 
#include <vector>
#include <unordered_set>
#include <mutex>
#include <stack>
#include <tuple>
#include <memory>

#define MAX_KILLS 6

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

  using position = std::pair<uint32_t, uint32_t>;

private:
  static constexpr uint32_t MAX_SIZE = (ROWS > COLS) ? ROWS : COLS;
  static constexpr uint32_t KILL_COEF[MAX_KILLS] = {1, 2, 3, 4, 5, 10};

  uint32_t identifier;
  std::string name;
  uint32_t last_frame_played;
  uint32_t frame_alive;
  position current_pos;
  float64_t p_score;
  direction last_direction;
  hsl_color color;
  bool connected;

  std::size_t last_region_size = 0;
  std::size_t last_region_size_ttl = 0;

  std::size_t n_kills;
  std::size_t n_trail_on_border;

  std::vector<std::shared_ptr<Action<ROWS, COLS>>> deconnected_actions{};
  std::size_t deconnected_action_index = 0;

  ConcurrentUnorderedSet<position, PairHash> trail{ };
  ConcurrentUnorderedSet<position, PairHash> region{ };
  std::mutex mu;

public:
  Player(const std::string& name, uint32_t id, hsl_color color, float64_t score, uint32_t frame)
    : identifier{ id }, name{ name }, last_frame_played{ frame }, frame_alive{ 0 }, current_pos{}, p_score{ score }, last_direction{ direction::DOWN },
      color{ color }, connected{ true }, n_kills{ 0 }, n_trail_on_border{ 0 } {
    trail.reserve(ROWS * COLS / 2);
    region.reserve(MAX_SIZE * 2);
  }


  std::string player_name() const noexcept { return name; }
  uint32_t id() const noexcept         { return identifier; }
  position pos() const noexcept            { return current_pos; }
  uint32_t tick_alive() const noexcept { return frame_alive; }

  void remove_region(const position& pos) {
    std::lock_guard<std::mutex> lock(mu);

    region.erase(pos);
  }

  void set_last_region_size(std::size_t size) noexcept {
    std::lock_guard<std::mutex> lock(mu);

    last_region_size = size;
    last_region_size_ttl = size;
  }


  void append_region(const std::vector<position>& new_region) {
    std::lock_guard<std::mutex> lock(mu);

    region.insert(new_region.begin(), new_region.end());
  }

  void set_connection(bool status) noexcept { connected = status; }
  void connect() noexcept { connected = true; }
  void disconnect() noexcept { connected = false; }
  bool is_connected() const noexcept { return connected; }

  boost::float64_t score() const noexcept { return p_score; }

  void increase_kill() noexcept {
    if (n_kills < MAX_KILLS)
      ++n_kills;
  }

  boost::float64_t frame_score() noexcept {
    if (frame_alive == 0 || last_region_size_ttl == 0) return 0.0;
    
    --last_region_size_ttl;

    boost::float64_t last_zone_completed = (last_region_size / static_cast<boost::float64_t>(ROWS * COLS)) * 100.00;

    boost::float64_t score = 100.0 * static_cast<boost::float64_t>(region.size());
    score *= last_zone_completed * last_zone_completed;
    score *= ((KILL_COEF[n_kills] * KILL_COEF[n_kills]) / static_cast<boost::float64_t>(frame_alive)) + 1;

    return score;
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

  movement_type play_deconnected_action(uint32_t frame) {
    if (frame <= last_frame_played)
      return movement_type::IDLE;

    last_frame_played = frame;
    ++frame_alive;

    return play(deconnected_actions[deconnected_action_index++]);
  }


  movement_type handle_action(uint32_t frame, const std::string& payload) {
    if (frame <= last_frame_played)
      return movement_type::IDLE;

    deconnected_action_index = 0;
    last_frame_played = frame;
    ++frame_alive;

    auto action = RetrieveAction<ROWS, COLS>()(payload);
    return play(action);
  }

  movement_type play(std::shared_ptr<Action<ROWS, COLS>> action) {
    if (auto pattern_action = std::dynamic_pointer_cast<PatternAction<ROWS, COLS>>(action)) {
      deconnected_actions = pattern_action->get_actions();
      return movement_type::IDLE;
    }

    auto new_pos = action->perform(current_pos);

    if (new_pos == pos()) {
      std::cout << "IDLE" << std::endl;
      return movement_type::IDLE;
    }


    current_pos = new_pos;

    if (trail.find(new_pos) != trail.end())
      return movement_type::DEATH;

    if (region.find(new_pos) != region.end()) 
      return movement_type::COMPLETE;

    
    trail.insert(new_pos);   
    p_score += frame_score(); 

    return movement_type::STEP;
  }

  void deplace(const position& p) {
    std::lock_guard<std::mutex> lock(mu);
    current_pos = p;
  }


  template<typename F>
  void for_each_region(F&& f) {
    region.for_each(f);
  }

  std::vector<position> get_region() {
    std::lock_guard<std::mutex> lock(mu);
    
    return std::vector<position>(region.get().begin(), region.get().end());
  }

  std::vector<position> get_trail() {
    std::lock_guard<std::mutex> lock(mu);
    
    return std::vector<position>(trail.get().begin(), trail.get().end());
  }

  template<typename F>
  void for_each_trail(F&& f) {
    trail.for_each(f);
  }

  void clear_trail() { trail.clear(); }

  void death() {  
    trail.clear();
    region.clear();

    std::lock_guard<std::mutex> lock(mu);
    frame_alive = 0;
    n_kills = 0;
    n_trail_on_border = 0;
  }

  void serialize(std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(mu);
    serialize_value<uint32_t>(data, player_name().size());
    serialize_data<std::string>(data, player_name(), player_name().size());
    serialize_value<hsl_color>(data, color);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint32_t>(data, frame_alive);

    serialize_value<uint32_t>(data, static_cast<uint32_t>(trail.size()));
    trail.for_each([&data](const position& p) {
      serialize_value<uint32_t>(data, p.first);
      serialize_value<uint32_t>(data, p.second);
    });

    serialize_value<uint32_t>(data, static_cast<uint32_t>(region.size()));
    region.for_each([&data](const position& p) {
      serialize_value<uint32_t>(data, p.first);
      serialize_value<uint32_t>(data, p.second);
    });
  }

private:
  movement_type teleport(position pos) {
    if (is_out_of_bound(pos) || !region.contains(pos))
      return TileMap::stmt::IDLE;

    clear_trail();

    current_pos = pos;
    return TileMap::stmt::STEP;
  }

  bool is_out_of_bound(const position& pos) const noexcept {
    return pos.first >= ROWS || pos.second >= COLS;
  }

  bool is_on_border(const position& pos) const noexcept {
    return pos.first == 0 || pos.second == 0 || pos.first == ROWS - 1 || pos.second == COLS - 1;
  }
};


#endif //SPACE_PLAYER_H