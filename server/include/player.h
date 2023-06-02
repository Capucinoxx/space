#ifndef SPACE_PLAYER_H
#define SPACE_PLAYER_H

#include "tile_map.h"
#include "utils.h"

#include <vector>

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
  std::string uuid;
  direction next_direction;
  position current_pos;
  std::vector<position> trail;


public:
  Player(const std::string& uuid) : uuid{ uuid } {
    // temporaire
    next_direction = DOWN;
    current_pos = { 2, 4 };
    trail.reserve(ROWS * COLS / 2);
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

  std::string id() const noexcept { return uuid; }
  position pos() const noexcept   { return current_pos; }

  trail_iterator begin() noexcept { return trail.begin(); }
  trail_iterator end() noexcept   { return trail.end(); }

  const_trail_iterator begin() const noexcept { return trail.begin(); }
  const_trail_iterator end() const noexcept   { return trail.end(); }

  movement_type update() {
    trail.push_back(current_pos);

    auto res = move(next_direction);

    return res;
  }

  void serialize(std::vector<uint8_t>& data) {
    serialize_data<std::string>(data, id(), UUID_SIZE);
    serialize_value<uint32_t>(data, pos().first);
    serialize_value<uint32_t>(data, pos().second);
    serialize_value<uint8_t>(data, static_cast<uint8_t>(begin() - end()));

    for (auto it = begin(); it != end(); ++it) {
      serialize_value<uint32_t>(data, it->first);
      serialize_value<uint32_t>(data, it->second);
    }
  }

private:
  movement_type move(direction d) noexcept {
    // todo: if out of bound, return IDLE


    switch (d) {
      case UP:    --current_pos.second; break;
      case DOWN:  ++current_pos.second; break;
      case LEFT:  --current_pos.first; break;
      case RIGHT: ++current_pos.first; break;
    }

    // todo: on aurait besoin de la map pour savoir le type de mouvement fait
    return movement_type::STEP;
  }
};

#endif //SPACE_PLAYER_H
