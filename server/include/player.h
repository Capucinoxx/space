#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"
#include "structures/concurent_unordered_set.h"
#include "utils.h"
#include "action.h"

#include <boost/cstdfloat.hpp> 
#include <ostream>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <stack>
#include <tuple>
#include <memory>

template<uint32_t ROWS, uint32_t COLS>
class PlayerScore {
public:
  uint64_t total_score;

  std::tuple<uint64_t, uint64_t, uint64_t> tick_score;

  explicit PlayerScore(uint64_t score) 
    : total_score{ score }, tick_score{  } { }

  uint64_t score() noexcept {
    total_score += std::get<0>(tick_score) + std::get<1>(tick_score) + std::get<2>(tick_score);
    tick_score = { 0, 0, 0 };
    return total_score;
  }

  void add_zone_score(uint64_t n_tiles) noexcept {
    std::get<0>(tick_score) += n_tiles;
  }

  void add_kill_score(uint64_t trail_size) noexcept {
    std::get<1>(tick_score) += (12 * (1 + trail_size));
  }

  void add_capture_score(uint64_t trail_size) noexcept {
    std::get<2>(tick_score) += (3 * (1 + trail_size));
  }

  friend std::ostream& operator<<(std::ostream &os, const PlayerScore& ps) {
    os  << " { total_score: " 
        << ps.total_score << ", tick_score: { zone_score: "  << std::get<0>(ps.tick_score) 
                                      << ", kill_score: " << std::get<1>(ps.tick_score) 
                                      << ", capture_score: " << std::get<2>(ps.tick_score) << " } }";
    return os;
  }
};


struct PairHash {
  template <typename T1, typename T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2;
  }
};

class TeleportStatement {
private:
  constexpr static uint8_t TTL = 8;
  uint8_t ttl = 0;

public:
  TeleportStatement() = default;

  bool can_teleport() const noexcept { return ttl == 0; }
  void decrement_ttl() noexcept {
    if (ttl > 0)
      --ttl;
  }

  void teleport() noexcept { ttl = TTL; }
};

template<uint32_t ROWS, uint32_t COLS>
class DeconnectedActions {
private:
  std::vector<std::shared_ptr<Action<ROWS, COLS>>> actions;
  std::size_t index = 0;

public:
  DeconnectedActions() 
    : actions{ std::make_shared<MovementAction<ROWS, COLS>>(std::to_string('\x01')) }, index{ 0 } {}

  DeconnectedActions(const std::vector<std::shared_ptr<Action<ROWS, COLS>>>& actions) 
    : actions{ actions }, index{ 0 } { }


  std::shared_ptr<Action<ROWS, COLS>> next() {
    return actions[index++ % actions.size()];
  }

  void update(const std::vector<std::shared_ptr<Action<ROWS, COLS>>>& new_actions) {
    actions = new_actions;
  }

  void reset() noexcept { index = 0; }
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
  uint32_t identifier;
  std::string name;
  uint32_t last_frame_played;
  uint32_t frame_alive;
  position current_pos;

  PlayerScore<ROWS, COLS> p_score;
  TeleportStatement teleport_statement{};
  DeconnectedActions<ROWS, COLS> deconnected_actions{ };

  direction last_direction;
  hsl_color color;
  bool connected;

  ConcurrentUnorderedSet<position, PairHash> trail{ };
  ConcurrentUnorderedSet<position, PairHash> region{ };
  std::mutex mu;

public:
  Player(const std::string& name, uint32_t id, hsl_color color, uint64_t score, uint32_t frame)
    : identifier{ id }, name{ name }, last_frame_played{ frame }, frame_alive{ 0 }, current_pos{}, p_score{ score }, last_direction{ direction::DOWN },
      color{ color }, connected{ true } {
    trail.reserve(ROWS * COLS / 2);
    region.reserve(((ROWS > COLS) ? ROWS : COLS) * 2);
  }

  // ==================================================================================
  // methods for player's attricutes
  // ==================================================================================

  std::string player_name() const noexcept { 
    return name;
  }
  
  uint32_t id() const noexcept {
    return identifier;
  }

  position pos() const noexcept {
    return current_pos;
  }
  void increase_frame_alive() noexcept {
    ++frame_alive;
  }

  uint32_t tick_alive() const noexcept {
    return frame_alive;
  }

  void connect() noexcept {
    connected = true;
  }

  void disconnect() noexcept {
    connected = false;
  }
  
  bool is_connected() const noexcept {
    return connected;
  }

  void clear() {
    last_frame_played = 0;
    region.clear();
    trail.clear();
  }


  // ==================================================================================
  // methods for deconnected actions
  // ==================================================================================

  void has_played() noexcept { 
    deconnected_actions.reset();
  }

  void update_pattern(const std::vector<std::shared_ptr<Action<ROWS, COLS>>>& new_pattern) noexcept {
    deconnected_actions.update(new_pattern);
  }

  std::shared_ptr<Action<ROWS, COLS>> next_disconnected_action() {
    return deconnected_actions.next();
  }


  // ==================================================================================
  // methods for score
  // ==================================================================================

  uint64_t tick_score() noexcept {
    p_score.add_zone_score(region.size());
    std::cout << p_score << std::endl;
    return p_score.score();
  }

  void zone_captured_bonus() noexcept { 
    p_score.add_capture_score(trail.size());
  }

  void kill_bonus() noexcept { 
    p_score.add_kill_score(trail.size());
  }

  uint64_t score() const noexcept {
    return p_score.total_score;
  }

  
  // ==================================================================================
  // methods for player's region
  // ==================================================================================

  template<typename F>
  void for_each_region(F&& f) {
    region.for_each(f);
  }

  void append_region(const std::vector<position>& new_region) {
    region.insert(new_region.begin(), new_region.end());
  }

  void remove_region(const position& pos) {
    region.erase(pos);
  }

  std::vector<position> get_region() {    
    return std::vector<position>(region.get().begin(), region.get().end());
  }


  // ==================================================================================
  // methods for player's trail
  // ==================================================================================

  template<typename F>
  void for_each_trail(F&& f) {
    trail.for_each(f);
  }

  std::vector<position> get_trail() {    
    return std::vector<position>(trail.get().begin(), trail.get().end());
  }

  void clear_trail() {
    trail.clear();
  }


  // ==================================================================================
  // methods for player's actions
  // ==================================================================================

  movement_type move(const position& new_pos) {
    if (new_pos == current_pos)
      return movement_type::IDLE;

    current_pos = new_pos;

    if (trail.find(new_pos) != trail.end())
      return movement_type::DEATH;

    if (region.find(new_pos) != region.end()) 
      return movement_type::COMPLETE;

    trail.insert(new_pos);
    return movement_type::STEP;
  }

  bool can_play(uint32_t frame) noexcept { 
    bool ok = frame > last_frame_played; 
    last_frame_played = frame;
    return ok;
  }

  void deplace(const position& p) {
    std::lock_guard<std::mutex> lock(mu);
    current_pos = p;
    
    if (!region.contains(p))
      trail.insert(p);
  }

  void death() {  
    trail.clear();
    region.clear();
    frame_alive = 0;
  }

  void teleport(const position& p) noexcept {
    if (!can_teleport(p))
      return;

    teleport_statement.teleport();
    deplace(p);
    clear_trail();
  }

  bool can_teleport(const position& p) const noexcept {
    return region.contains(p) && teleport_statement.can_teleport();
  }
  

  // ==================================================================================
  // methods for player's informations
  // ==================================================================================  

  void dump_info() {
    std::cout << "Player: " << name << std::endl;
    std::cout << "  - ID: " << identifier << std::endl;
    std::cout << "  - Last frame played: " << last_frame_played << std::endl;
    std::cout << "  - Frame alive: " << frame_alive << std::endl;
    std::cout << "  - Current position: (" << current_pos.first << ", " << current_pos.second << ")" << std::endl;
    std::cout << "  - Last direction: " << last_direction << std::endl;
    std::cout << "  - Trail size: " << trail.size() << std::endl;
    std::cout << "  - Region size: " << region.size() << std::endl;
    std::cout << "  - Score: " << p_score << std::endl;
    std::cout << std::endl;
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
};


#endif //SPACE_PLAYER_H