#ifndef SPACE_PLAYER_PLAYER_H
#define SPACE_PLAYER_PLAYER_H

#include "common.h"
#include "player/deconnected_actions.h"
#include "player/teleport_stmt.h"
#include "player/player_score.h"
#include "tile_map.h"

#include <boost/cstdfloat.hpp> 
#include <unordered_set>
#include <memory>
#include <algorithm>

class player;
using player_sptr = std::shared_ptr<player>;

using position = std::pair<uint32_t, uint32_t>;

class player {
public:
  enum direction { UP, DOWN, LEFT, RIGHT };

  using hsl_color = std::tuple<boost::float64_t, boost::float64_t, boost::float64_t>;
  using action_stmt = TileMap::stmt;


private:
  uint32_t uid;
  std::string name;
  uint32_t last_tick;
  uint32_t n_tick_alive;
  position current_pos;

  player_score score;
  teleport_stmt teleport_stmt_{ };
  deconnected_actions deconnected_actions_{ };

  hsl_color hsl;
  bool connected;

  std::unordered_set<position, pair_hash> trail{ };
  std::unordered_set<position, pair_hash> region{ };

public:
  player(std::string name, uint32_t uid, hsl_color color, uint64_t score, uint32_t tick)
    : uid{ uid }, name{ std::move(name) }, last_tick{ tick }, n_tick_alive{ 0 }, current_pos{ 0, 0 }, score{ score }, hsl{ color }, connected{ true } {
      trail.reserve(rows * cols / 2);
      region.reserve(((rows > cols) ? rows : cols) * 2);
  }

  // ==================================================================================
  // methods for player's attricutes
  // ==================================================================================

  std::string player_name()   const noexcept { return name; }
  uint32_t    id()            const noexcept { return uid; }
  position    pos()           const noexcept { return current_pos; }
  uint32_t    tick_alive()    const noexcept { return n_tick_alive; }
  bool        is_connected()  const noexcept { return connected; }

  void increase_tick_alive()  noexcept { ++n_tick_alive; }
  void connect()              noexcept { connected = true; }
  void disconnect()           noexcept { connected = false; }

  void clear() {
    n_tick_alive = 0;
    trail.clear();
    region.clear();
  }


  // ==================================================================================
  // methods for deconnected actions
  // ==================================================================================

  void has_played() noexcept { deconnected_actions_.reset(); }

  void update_pattern(const std::vector<std::shared_ptr<Action<rows, cols>>>& actions) noexcept {
    deconnected_actions_.update(actions);
  }

  std::shared_ptr<Action<rows, cols>> next_disconnected_action() noexcept {
    return deconnected_actions_.next();
  }


  // ==================================================================================
  // methods for score
  // ==================================================================================

  uint64_t tick_score() noexcept {
    score.add_zone_score(region.size());
    std::cout << score << std::endl;
    return score.score();
  }

  void zone_captured_bonus() noexcept { 
    score.add_capture_score(trail.size());
  }

  void kill_bonus() noexcept { 
    score.add_kill_score(trail.size());
  }

  uint64_t total_score() const noexcept {
    return score.total_score;
  }


  // ==================================================================================
  // methods for player's region
  // ==================================================================================

  template<typename F>
  void for_each_region(F&& f) noexcept {
    std::for_each(region.begin(), region.end(), f);
  }

  void append_region(const std::vector<position>& new_region) {
    region.insert(new_region.begin(), new_region.end());
  }

  void remove_region(const position& pos) {
    region.erase(pos);
  }

  std::vector<position> get_region() const noexcept {
    return std::vector<position>(region.begin(), region.end());
  }


  // ==================================================================================
  // methods for player's trail
  // ==================================================================================

  template<typename F>
  void for_each_trail(F&& f) noexcept {
    std::for_each(trail.begin(), trail.end(), f);
  }

  std::vector<position> get_trail() const noexcept {
    return std::vector<position>(trail.begin(), trail.end());
  }

  void clear_trail() {
    trail.clear();
  }


  // ==================================================================================
  // methods for player's actions
  // ==================================================================================

  action_stmt move(const position& new_pos) {
    if (new_pos == current_pos)
      return action_stmt::IDLE;

    current_pos = new_pos;

    if (trail.find(new_pos) != trail.end())
      return action_stmt::DEATH;

    if (region.find(new_pos) != region.end())
      return action_stmt::COMPLETE;

    trail.insert(new_pos);
    return action_stmt::STEP;
  }

  bool can_play(uint32_t tick) noexcept {
    bool ok = tick > last_tick;
    last_tick = tick;
    teleport_stmt_.tick();
    return ok;
  }

  void go_to(const position& pos) noexcept {
    current_pos = pos;
    if (region.find(pos) == region.end())
      trail.insert(pos);
  }

  void death() {
    trail.clear();
    region.clear();
    n_tick_alive = 0;
  }

  void teleport(const position& pos) noexcept {
    if (!can_teleport(pos))
      return;

    teleport_stmt_.teleport();
    go_to(pos);
    clear_trail();
  }

  bool can_teleport(const position& pos) const noexcept {
    return teleport_stmt_.can_teleport() && region.find(pos) != region.end();
  }


  // ==================================================================================
  // methods for player's informations
  // ==================================================================================  

  void dump_info() {
    std::cout << "Player: " << name << std::endl;
    std::cout << "  - ID: " << uid << std::endl;
    std::cout << "  - Last frame played: " << last_tick << std::endl;
    std::cout << "  - Frame alive: " << n_tick_alive << std::endl;
    std::cout << "  - Current position: (" << current_pos.first << ", " << current_pos.second << ")" << std::endl;
    std::cout << "  - Trail size: " << trail.size() << std::endl;
    std::cout << "  - Region size: " << region.size() << std::endl;
    std::cout << "  - Score: " << score << std::endl;
    std::cout << std::endl;
  }

  void serialize(std::vector<uint8_t>& data) {
    serialize_value<uint32_t>(data, player_name().size());
    serialize_data<std::string>(data, player_name(), player_name().size());
    serialize_value<hsl_color>(data, hsl);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint32_t>(data, tick_alive());
    serialize_value<uint8_t>(data, teleport_stmt_.cooldown());

    serialize_value<uint32_t>(data, static_cast<uint32_t>(trail.size()));
    std::for_each(trail.begin(), trail.end(), [&data](const position& p) {
      serialize_value<uint32_t>(data, p.first);
      serialize_value<uint32_t>(data, p.second);
    });

    serialize_value<uint32_t>(data, static_cast<uint32_t>(region.size()));
    std::for_each(region.begin(), region.end(), [&data](const position& p) {
      serialize_value<uint32_t>(data, p.first);
      serialize_value<uint32_t>(data, p.second);
    });
  }
};

#endif //SPACE_PLAYER_PLAYER_H