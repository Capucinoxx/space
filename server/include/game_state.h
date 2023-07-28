#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include "player/player.h"
#include "grid.h"
#include "common.h"
#include "postgres_connector.h"
#include "structures/concurrent_unordered_map.h"
#include "structures/concurrent_unordered_set.h"
#include "structures/concurrent_queue.h"
#include "action.h"

#include <thread>
#include <atomic>
#include <vector>
#include <type_traits>
#include <utility>
#include <optional>
#include <mutex>



template<uint32_t ROWS, uint32_t COLS>
class Spawner {
  public:
    virtual std::pair<uint32_t, uint32_t> operator()() = 0;
    virtual ~Spawner() = default;
};


class multiplier_statement {
private:
  uint64_t value{ 1 };
  std::mutex mu;

public:
  void operator++() { 
    std::lock_guard<std::mutex> lock(mu);
    ++value;
  }

  void operator--() { 
    std::lock_guard<std::mutex> lock(mu);
    if (value == 1)
      return;

    --value;
  }

  uint64_t operator()() noexcept { 
    std::lock_guard<std::mutex> lock(mu);
    return value;
  }
};

template<uint32_t ROWS, uint32_t COLS>
class Spawn : public Spawner<ROWS, COLS> {
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

  std::pair<uint32_t, uint32_t> operator()() override {
    uint32_t x = dist_x(gen);
    uint32_t y = dist_y(gen);

    int retry = 0;

    while (!position_is_valid({ x, y }) && retry++ < 10) {
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

        if (grid->is_invalid_pos({ px, py }))
          return false;
      }
    }

    return true;
  }
};

template<typename T, uint32_t ROWS, uint32_t COLS>
class GameState {
  static_assert(std::is_same<T, std::pair<typename T::first_type, typename T::second_type>>::value, "T must be a pair");
  static_assert(std::is_same<typename T::first_type, std::shared_ptr<player>>::value,
                  "T::first_type must be std::shared_ptr<Player>");

public:
  using psql_ref = std::shared_ptr<PostgresConnector>;
  using player_t = player;
  using player_ptr = std::shared_ptr<player_t>;

private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  c_unordered_map<uint32_t, player_ptr> players;
  c_unordered_set<uint32_t> inactive_players;
  
  psql_ref psql;
  Spawner<ROWS, COLS>* spawn;
  uint32_t frame_count{ 1 };
  c_queue<T> actions{ };

  bool score_insert;

  static UniqueIDGenerator<15> uuid_generator;
  multiplier_statement multiplier{ };


public:
  explicit GameState(psql_ref psql, bool score_insert) 
    : grid(std::make_shared<Grid<ROWS, COLS>>()), psql(psql), spawn(new Spawn<ROWS, COLS>(grid)), score_insert(score_insert) {}

  explicit GameState(psql_ref psql, Spawner<ROWS, COLS>* spawn, bool score_insert) 
    : grid(std::make_shared<Grid<ROWS, COLS>>()), psql(psql), spawn(spawn), score_insert(score_insert) {}

  ~GameState() = default;

  player_ptr register_player(const std::string& name, uint32_t id, player_t::hsl_color color, uint64_t score) {
    return register_player(name, id, color, score, (*spawn)());
  }

  player_ptr register_player(const std::string& name, uint32_t id, player_t::hsl_color color, uint64_t score, std::pair<uint32_t, uint32_t> spawn_position) {
    auto it = players.find(id);
    if (it != players.end()) {
      if (it->second->is_connected())
        return nullptr;

      it->second->connect();
      return it->second;
    }

    auto p = std::make_shared<player_t>(name, id, color, score, 0);
    spawn_player(p, spawn_position);
    players.insert(id, p);

    // inactive_players.insert(id);

    return p;
  }

  uint64_t increase_multiplier() {
    ++multiplier;
    return multiplier();
  }

  uint64_t decrease_multiplier() {
    --multiplier;
    return multiplier();
  }

  static std::string generate_secret() { return uuid_generator(); }
  
  void push(T&& action) { actions.push(std::move(action)); }
  
  uint32_t frame() const noexcept { return frame_count; }

  void disconnect_player(uint32_t id) {
    auto it = players.find(id);
    if (it != players.end())
      it->second->disconnect();
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

  void tick() {
    std::unordered_set<uint32_t> ids{};

    actions.for_each([this, &ids](const T& tick_action) {
      auto& [player, payload] = tick_action;
      ids.insert(player->id());
      play_tick(tick_action);
      player->has_played();

    });

    for (auto& player : players) {
      if (!inactive_players.contains(player.first)) {
        player.second->increase_tick_alive();
        if (ids.find(player.first) == ids.end())
          play_tick(std::make_pair(player.second, std::string{}));
      }
    }

    store_scores(psql != nullptr && score_insert);

    inactive_players.clear();
    ++frame_count;
  }

  void play_tick(const T& tick_action) {
    auto& [player, payload] = tick_action;

    if (!player->can_play(frame()))
      return;

    auto action = retrieve_action(player, payload);

    if (update_pattern(player, action))
      return;

    auto old_pos = player->pos();
    auto new_pos = action->perform(old_pos);

    if (grid->is_invalid_pos(new_pos))
      return;

    
    handle_teleportation(player, action, new_pos) || handle_move_result(player, action, new_pos) || handle_step_statement(player, new_pos);
  }

  tile_map& cell(position pos) const {
    return grid->at(pos);
  }

  void clear() {
    grid->clear();
    for (auto& player : players)
      player.second->clear();

    inactive_players.clear();
    frame_count = 1;

    for (auto& player : players) {
      inactive_players.insert(player.first);
      spawn_player(player.second, (*spawn)());
    }
  }

private:
  void kill(player_ptr murder, player_ptr victim) {
    victim->dump_info();

    auto victim_id = victim->id();
    
    victim->for_each_region([grid = grid, victim_id](position p) { grid->at(p).reset(victim_id); });
    victim->for_each_trail([grid = grid, victim_id](position p) { grid->at(p).reset(victim_id); });
    victim->death();

    spawn_player(victim, (*spawn)());

    inactive_players.insert(victim->id());

    if (victim->id() != murder->id())
      murder->kill_bonus();
  }

  void spawn_player(player_ptr player, const std::pair<uint32_t, uint32_t>& pos) {
    std::vector<std::pair<uint32_t, uint32_t>> positions;

    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto p = std::make_pair(pos.first + i, pos.second + j);
        if (grid->is_invalid_pos(p))
          continue;

        positions.push_back(p);

        auto [statement, victim_id] = grid->at(p).take(player->id());
        if (victim_id == 0)
          continue;

        auto victim = players.find(victim_id)->second;
        if (statement == tile_map::stmt::DEATH || (p == victim->pos()))
            kill(player, victim);                 
        else if (victim_id != player->id())
          victim->remove_region(p);
      }
    }

    player->append_region(positions);
    player->go_to(pos);
  }

  void investigate_captured_tiles(const player_ptr& player, const std::unordered_map<uint32_t, std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash>>& player_tiles) {
    for (auto& [player_id, tiles] : player_tiles) {
      auto victim = players.find(player_id)->second;

      if (tiles.find(victim->pos()) != tiles.end()) {
        kill(player, victim);
        continue;
      }

      for (const auto& tile : tiles)
        victim->remove_region(tile);
    }
  }

  void store_scores(bool with_insert) {
    std::vector<std::string> arguments;
    
    std::string query = "INSERT INTO player_scores (player_id, score) VALUES ";
    std::size_t i = 0;

    for (auto& player : players) {
      arguments.emplace_back(std::to_string(player.second->id()));
      arguments.emplace_back(std::to_string(player.second->tick_score(multiplier())));

      if (with_insert) {
        query += "($" + std::to_string(i * 2 + 1) + ", $" + std::to_string(i * 2 + 2) + ")";
        if (i != players.size() - 1)
          query += ", ";

        ++i;
      }
    }

    if (arguments.size() > 0 && with_insert)
      psql->bulk_insert(query, arguments);
  }

  bool handle_teleportation(const player_ptr& player, const std::shared_ptr<Action<ROWS, COLS>>& action, const position& new_pos) {
    if (auto teleport_action = std::dynamic_pointer_cast<TeleportAction<ROWS, COLS>>(action)) {
      if (!player->can_teleport(new_pos))
        return true;

      if (grid->at(new_pos).is_step())
        return true;

      player->for_each_trail([grid = grid, player_id = player->id()](position p) { 
        grid->at(p).reset(player_id); });

      player->teleport(new_pos);
      return true;
    }
    return false;
  }

  bool handle_move_result(const player_ptr& player, const std::shared_ptr<Action<ROWS, COLS>>& action, const position& new_pos) {
    switch (player->move(new_pos)) {
      case player_t::action_stmt::DEATH:
        kill(player, player);
        return true;

      case player_t::action_stmt::COMPLETE:
        if (player->get_trail().empty())
          return false;

        player->zone_captured_bonus();
        investigate_captured_tiles(player, grid->fill_region(player));
        player->clear_trail();
        return false;
        
      default:
        return false;
    }
  }

  bool handle_step_statement(const player_ptr& player, const position& new_pos) {
    auto [statement, victim_id] = grid->at(new_pos).step(player->id());
    return handle_statement(player, new_pos, statement, victim_id);
  }

  bool handle_statement(const player_ptr& player, const position& new_pos, player_t::action_stmt statement, uint32_t victim_id) {
    switch (statement) {
      case player_t::action_stmt::DEATH:
        kill(player, players.find(victim_id)->second);
        return true;

      case player_t::action_stmt::COMPLETE:
        if (player->get_trail().empty())
          return true;

        player->zone_captured_bonus();
        investigate_captured_tiles(player, grid->fill_region(player));
        player->clear_trail();
        return true;
      
      case player_t::action_stmt::STEP:
        if (victim_id != 0 && victim_id != player->id())
          players.find(victim_id)->second->remove_region(player->pos());
        
        player->go_to(new_pos);
        return true;

      default:
        return false;
    }
  }

  std::shared_ptr<Action<ROWS, COLS>> retrieve_action(const player_ptr& player, const std::string& payload) {
    if (payload.empty())
      return player->next_disconnected_action();

    return RetrieveAction<ROWS, COLS>()(payload);
  }

  bool update_pattern(const player_ptr& player, const std::shared_ptr<Action<ROWS, COLS>>& action) {
    if (auto pattern_action = std::dynamic_pointer_cast<PatternAction<ROWS, COLS>>(action)) {
      player->update_pattern(pattern_action->get_actions());
      return true;
    }

    return false;
  }
};

template<typename T, uint32_t ROWS, uint32_t COLS>
UniqueIDGenerator<15> GameState<T, ROWS, COLS>::uuid_generator{}; 

using game_sptr = std::shared_ptr<GameState<std::pair<std::shared_ptr<player>, std::string>, rows, cols>>;

#endif //SPACE_MAP_H
