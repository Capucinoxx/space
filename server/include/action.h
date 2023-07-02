#ifndef SPACE_ACTION_H
#define SPACE_ACTION_H

#include <iostream>
#include <type_traits>
#include <memory>

template <uint32_t ROWS, uint32_t COLS>
class Action {
public:
  virtual std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept = 0;
  virtual ~Action() = default;
};

template <uint32_t ROWS, uint32_t COLS>
class MovementAction : public Action<ROWS, COLS> {
public:
  enum direction { UP, DOWN, LEFT, RIGHT };

  MovementAction(uint8_t data) {
    switch (data) {
      case (uint8_t)0: dir = direction::UP; break;
      case (uint8_t)1: dir = direction::DOWN; break;
      case (uint8_t)2: dir = direction::LEFT; break;
      case (uint8_t)3: dir = direction::RIGHT; break;
      default: dir = direction::DOWN; break;
    }
  }

  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    auto new_pos = pos;

    switch (dir) {
      case UP:    --new_pos.second; break;
      case DOWN:  ++new_pos.second; break;
      case LEFT:  --new_pos.first; break;
      case RIGHT: ++new_pos.first; break;
    }

    if (new_pos.first >= ROWS || new_pos.second >= COLS)
      return pos;

    return new_pos;
  }

private:
  direction dir;
};

template <uint32_t ROWS, uint32_t COLS>
class TeleportAction : public Action<ROWS, COLS> {
public:
  TeleportAction(const std::string& data) {
    if (!is_valid_payload(data)) {
      is_valid = false;
      return;
    }

    extract_position(data);
  }

  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    if (!is_valid || x >= ROWS || y >= COLS)
      return pos;

    return { x, y };
  }

private:
  uint32_t x;
  uint32_t y;

  bool is_valid = true;

  bool is_valid_payload(const std::string& data) {
    return data.size() == 9 && data[0] == 0x05;
  }

  void extract_position(const std::string& payload) {
    x = static_cast<uint32_t>(payload[4] << 24 | payload[3] << 16 | payload[2] << 8 | payload[1]);
    y = static_cast<uint32_t>(payload[8] << 24 | payload[7] << 16 | payload[6] << 8 | payload[5]);
  }
};

template <uint32_t ROWS, uint32_t COLS>
class UndefinedAction : public Action<ROWS, COLS> {
public:
  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    return pos;
  }
};


template <uint32_t ROWS, uint32_t COLS>
struct RetrieveAction {
  std::pair<uint32_t, uint32_t> operator()(const std::string& data, const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    if (data.empty()) 
      return UndefinedAction<ROWS, COLS>().perform(pos);

    if (data[0] <= 0x03)
      return MovementAction<ROWS, COLS>(data[0]).perform(pos);

    if (data[0] == 0x05)
      return TeleportAction<ROWS, COLS>(data).perform(pos);

    return UndefinedAction<ROWS, COLS>().perform(pos);
  }
};


#endif // SPACE_ACTION_H
