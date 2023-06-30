// #ifndef SPACE_MAP_H
// #define SPACE_MAP_H

// #include "player.h"
// #include "grid.h"
// #include "utils.h"
// #include "postgres_connector.h"
// #include "structures/concurrent_unordered_map.h"

// #include <iostream> 
// #include <thread>
// #include <atomic>
// #include <random>
// #include <string>


// template<uint32_t ROWS, uint32_t COLS>
// class Spawn {
// private:
//   std::shared_ptr<Grid<ROWS, COLS>> grid;

//   std::random_device rd;
//   std::mt19937 gen;
//   std::uniform_int_distribution<uint32_t> dist_x;
//   std::uniform_int_distribution<uint32_t> dist_y;

// public:
//   Spawn(std::shared_ptr<Grid<ROWS, COLS>> grid) : grid(std::move(grid)), gen{ rd() } { 
//     dist_x = std::uniform_int_distribution<uint32_t>(0, ROWS - 1);
//     dist_y = std::uniform_int_distribution<uint32_t>(0, COLS - 1);
//   }

//   std::pair<uint32_t, uint32_t> operator()() {
//     uint32_t x = dist_x(gen);
//     uint32_t y = dist_y(gen);

//     int retry = 0;

//     while (position_is_valid({ x, y }) && retry++ < 10) {
//       x = dist_x(gen);
//       y = dist_y(gen);
//     }

//     return { x, y };
//   }

//   bool position_is_valid(const std::pair<uint32_t, uint32_t>& pos) {
//     for (int i = -1; i != 2; ++i) {
//       for (int j = -1; j != 2; ++j) {
//         auto px = pos.first + i;
//         auto py = pos.second + j;

//         bool is_out_of_bounds = px >= ROWS || py >= COLS;
//         if (is_out_of_bounds)
//           return true;
//       }
//     }

    

//     return false;
//   }
// };

// template<uint32_t ROWS, uint32_t COLS>
// class GameManager {
// public:
//   using position = Player<ROWS, COLS>::position;
//   using broadcast_fn_type = void (*)(const std::vector<uint8_t>&);
//   using psql_ref = PostgresConnector&;
  

// private:
//   std::shared_ptr<Grid<ROWS, COLS>> grid;
//   std::shared_ptr<GameManager<ROWS, COLS>> game_manager;

//   ConcurrentUnorderedMap<uint32_t, std::shared_ptr<Player<ROWS, COLS>>> players{ };
//   Spawn<ROWS, COLS> spawn;

//   psql_ref postgres;
  
//   std::thread th;
//   uint32_t frame_count = 0;
//   std::atomic<bool> running{ false };

//   UniqueIDGenerator<15> uuid_generator;

// public:
//   explicit GameManager(psql_ref postgres) : grid(std::make_shared<Grid<ROWS, COLS>>()), spawn(grid), postgres(postgres) {}

//   ~GameManager() = default;
//   GameManager(const GameManager&) = delete;
//   GameManager& operator=(const GameManager&) = delete;

//   std::shared_ptr<Player<ROWS, COLS>> register_player(const std::string& name, uint32_t id, Player<ROWS, COLS>::hsl_color color) {
//     auto it = players.find(id);
//     if (it != players.end()) {
//       if (it->second->is_connected())
//         return nullptr;

//       it->second->set_connection(true);
//       return it->second;
//     }

//     auto p = std::make_shared<Player<ROWS, COLS>>(name, id, color, 0.0, frame_count);
//     spawn_player(p);
//     players.insert(id, p);

//     return p;
//   }

//   bool is_running() const noexcept { return running.load(); }
//   uint32_t frame() const noexcept { return frame_count; }

//   void remove_player(std::shared_ptr<Player<ROWS, COLS>> p) {
//     auto it = players.find(p->id());
//     if (it != players.end())
//       it->second->set_connection(false);
//   }

//   std::string generate_secret() { return uuid_generator(); }

//   template<typename T>
//   void start(T* object, void (T::*callback)(const std::vector<uint8_t>&)) { 
//     if (running.load())
//       return;

//     running.store(true);
//     th = std::thread([object, callback, this]() {
//       while(running) {
//         update_map();
//         auto data = serialize();
//         (object->*callback)(data);

//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//       }
//     });
//   }
  
//   void stop()  { 
//     running.store(false);
//     if (th.joinable())
//       th.join();
//   }
  
//   std::shared_ptr<Grid<ROWS, COLS>> get_grid() const noexcept {
//     return grid;
//   }

//   void spawn_player(std::shared_ptr<Player<ROWS, COLS>> p) {
//     p->spawn(spawn());
//   }

//   void handle_move_result(std::shared_ptr<Player<ROWS, COLS>> p, Player<ROWS, COLS>::movement_type movement) {
//     switch (movement) {
//       case Player<ROWS, COLS>::movement_type::DEATH:
//         kill(p->id(), p->id());
//         break;
//       case Player<ROWS, COLS>::movement_type::COMPLETE:
//         grid->fill_region(p);
//         break;
      

//       default:
//         grid->at(p->pos()).step(p->id());
//     }
//   }

//   void kill(uint32_t killer_id, uint32_t victim_id) {
//     auto victim = players.find(victim_id)->second;
//     auto killer = players.find(killer_id)->second;

//     std::cout << "--- " << victim_id << " KILL by " << killer_id << " ---" << std::endl; 

//     victim->dump_info();
//     victim->death();



//     spawn_player(victim);

//     if (killer_id != victim_id)
//       killer->increase_kill();

//   }

//   std::vector<uint8_t> serialize() {
//     std::vector<uint8_t> data;

//     serialize_value<uint32_t>(data, ROWS);
//     serialize_value<uint32_t>(data, COLS);
//     serialize_value<uint32_t>(data, frame());

//     for (const auto& player : players)
//       player.second->serialize(data);

//     return data;
//   }

// private:

// void update_map() {
//   std::vector<std::string> arguments;
//   std::string query = "INSERT INTO player_scores (player_id, score) VALUES ";
//   std::size_t i = 0;

//   for (auto& p : players) {
//     auto res = p.second->perform(frame_count);


//     handle_move_result(p.second, res);
//     arguments.emplace_back(std::to_string(p.second->id()));
//     arguments.emplace_back(std::to_string(p.second->score()));

//     query += "($" + std::to_string(i * 2 + 1) + ", $" + std::to_string(i * 2 + 2) + ")";
//     if (i != players.size() - 1)
//       query += ", ";

//     ++i;
//   }

//   postgres.bulk_insert(query, arguments);

//   ++frame_count;
// }

// };

// #endif //SPACE_MAP_H
