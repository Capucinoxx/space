#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include "player.h"
#include "grid.h"
#include "utils.h"
#include "postgres_connector.h"
#include "structures/concurrent_unordered_map.h"
#include "structures/concurrent_queue.h"
#include "action.h"

#include <thread>
#include <atomic>
#include <vector>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <unordered_set>

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
  static_assert(std::is_same<typename T::first_type, std::shared_ptr<Player<ROWS, COLS>>>::value,
                  "T::first_type must be std::shared_ptr<Player>");

public:
  using psql_ref = std::shared_ptr<PostgresConnector>;
  using player_t = Player<ROWS, COLS>;
  using player_ptr = std::shared_ptr<player_t>;

private:
  std::shared_ptr<Grid<ROWS, COLS>> grid;
  ConcurrentUnorderedMap<uint32_t, player_ptr> players;
  ConcurrentUnorderedSet<uint32_t> inactive_players;
  Spawn<ROWS, COLS> spawn;

  psql_ref psql;
  uint32_t frame_count{ 1 };
  c_queue<T> actions{ };

  static UniqueIDGenerator<15> uuid_generator;

public:
  explicit GameState(psql_ref psql) 
    : grid(std::make_shared<Grid<ROWS, COLS>>()), spawn(grid), psql(psql) { }

  ~GameState() = default;

  player_ptr register_player(const std::string& name, uint32_t id, player_t::hsl_color color, boost::float64_t score) {
    return register_player(name, id, color, score, spawn());
  }

  player_ptr register_player(const std::string& name, uint32_t id, player_t::hsl_color color, boost::float64_t score, std::pair<uint32_t, uint32_t> spawn_position) {
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
    grid->serialize(data);

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
    });

    for (auto& player : players) {
      if (!inactive_players.contains(player.first)) {
        player.second->increase_frame_alive();
        if (ids.find(player.first) == ids.end())
          play_tick(std::make_pair(player.second, std::string{}));
      }
    }

    if (psql != nullptr)
      store_scores();

    inactive_players.clear();
    ++frame_count;
  }

  void play_tick(const T& tick_action) {
    auto& [player, payload] = tick_action;
    if (!player->can_play(frame())) {
      return;
    }

    std::shared_ptr<Action<ROWS, COLS>> action;

    if (payload.empty()) {
      action = player->next_disconnected_action();
    } else {
      action = RetrieveAction<ROWS, COLS>()(payload);

      if (auto pattern_action = std::dynamic_pointer_cast<PatternAction<ROWS, COLS>>(action)) {
        player->update_pattern(pattern_action->get_actions());
        return;
      }
    }

    auto old_pos = player->pos();
    auto new_pos = action->perform(old_pos);

    if (grid->is_invalid_pos(new_pos)) {
      return;
    }

    switch (player->move(new_pos)) {
      case player_t::movement_type::DEATH:
        kill(player, player);
        return;

      case player_t::movement_type::COMPLETE:
        grid->fill_region(player);
        break;

      default: 
        break;
    }

    auto [statement, victim_id] = grid->at(new_pos).step(player->id());
    switch (statement) {
      case player_t::movement_type::DEATH:
        kill(player, players.find(victim_id)->second);
        break;

      case player_t::movement_type::COMPLETE:
        grid->fill_region(player);
        break;

      case player_t::movement_type::STEP:
        if (victim_id != 0 && victim_id != player->id())
          players.find(victim_id)->second->remove_region(player->pos());
        
        player->deplace(new_pos);
        break;

      default:
        break;
    }   
  }

  TileMap& cell(player_t::position pos) const {
    return grid->at(pos);
  }

  void clear() {
    grid->clear();
    for (auto& player : players)
      player.second->clear();

    inactive_players.clear();
  }

private:
  void handle_move_result(player_ptr player, player_t::movement_type movement) {
    switch (movement) {
      case player_t::movement_type::DEATH: 
        kill(player, player);
        break;

      case player_t::movement_type::COMPLETE:
        grid->fill_region(player);
        break;

      case player_t::movement_type::STEP:

      default:
        auto [statement, victim_id] = grid->at(player->pos()).step(player->id());
        if (victim_id != 0 && victim_id != player->id())
          players.find(victim_id)->second->remove_region(player->pos());

        if (statement == TileMap::stmt::DEATH)
          kill(player, players.find(victim_id)->second);

        
        break;
    }
  }

  void kill(player_ptr murder, player_ptr victim) {
    victim->dump_info();

    auto victim_id = victim->id();
    
    victim->for_each_region([grid = grid, victim_id](player_t::position p) { grid->at(p).reset(victim_id); });
    victim->for_each_trail([grid = grid, victim_id](player_t::position p) { grid->at(p).reset(victim_id); });
    victim->death();

    spawn_player(victim, spawn());

    inactive_players.insert(victim->id());

    if (victim->id() != murder->id())
      murder->increase_kill();
  }

  void spawn_player(player_ptr player, const std::pair<uint32_t, uint32_t>& pos) {
    std::vector<std::pair<uint32_t, uint32_t>> positions;

    for (int i = -1; i != 2; ++i) {
      for (int j = -1; j != 2; ++j) {
        auto p = std::make_pair(pos.first + i, pos.second + j);
        if (grid->is_invalid_pos(p))
          continue;

        auto [statement, victim_id] = grid->at(p).take(player->id());
        if (statement == TileMap::stmt::DEATH)
          kill(player, players.find(victim_id)->second);
        else if (statement == TileMap::stmt::STEP) {
          if (victim_id != 0 && victim_id != player->id())
            players.find(victim_id)->second->remove_region(player->pos());
        }
        positions.push_back(p);
      }
    }

    player->append_region(positions);
    player->deplace(pos);
  }

  void store_scores() {
    std::vector<std::string> arguments;
    
    std::string query = "INSERT INTO player_scores (player_id, score) VALUES ";
    std::size_t i = 0;

    for (auto& player : players) {
      arguments.emplace_back(std::to_string(player.second->id()));
      arguments.emplace_back(std::to_string(player.second->score()));

      query += "($" + std::to_string(i * 2 + 1) + ", $" + std::to_string(i * 2 + 2) + ")";
      if (i != players.size() - 1)
        query += ", ";

      ++i;
    }

    if (arguments.size() > 0)
      psql->bulk_insert(query, arguments);
  }
};

template<typename T, uint32_t ROWS, uint32_t COLS>
UniqueIDGenerator<15> GameState<T, ROWS, COLS>::uuid_generator{};  

#endif //SPACE_MAP_H
