#ifndef SPACE_MAP_H
#define SPACE_MAP_H

#include "player.h"
#include "grid.h"
#include "utils.h"
#include "postgres_connector.h"
#include "structures/concurrent_unordered_map.h"
#include "structures/concurrent_queue.h"

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
        if (is_out_of_bounds)
          return true;
      }
    }

    return false;
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
    p->spawn(spawn_position);
    players.insert(id, p);
    inactive_players.insert(id);

    return p;
  }

  static std::string generate_secret() { return uuid_generator(); }

  void push(T&& action) {
    actions.push(std::move(action));
  }

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

  void play_tick() {
    std::unordered_set<uint32_t> ids;

    actions.for_each([&](const T& action) {
      auto& [player, payload] = action;
      
      auto res = player->handle_action(frame(), payload);
      handle_move_result(player, res);
      ids.insert(player->id());
    });

    for (auto& player : players) {
      if (ids.find(player.first) == ids.end() && !inactive_players.contains(player.first)) {
        auto res = player.second->perform(frame());
        handle_move_result(player.second, res);
      }
    }

    if (psql != nullptr)
      store_scores();

    inactive_players.clear();
    ++frame_count;
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

      default:
        grid->at(player->pos()).step(player->id());
    }
  }

  void kill(player_ptr murder, player_ptr victim) {
    std::cout << "DEATH ! " << murder->id() << " KILL " << victim->id() << std::endl;
    victim->dump_info();
    victim->death();
    victim->for_each_region([this](player_t::position p) { grid->at(p).reset(); });
    victim->for_each_trail([this](player_t::position p) { grid->at(p).reset(); });


    victim->spawn(spawn());

    inactive_players.insert(victim->id());

    if (victim->id() != murder->id())
      murder->increase_kill();
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
