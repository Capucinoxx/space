#ifndef SPACE_ACTION_H
#define SPACE_ACTION_H

#include <iostream>
#include <type_traits>
#include <memory>

template <uint32_t ROWS, uint32_t COLS>
struct RetrieveAction;

template <uint32_t ROWS, uint32_t COLS>
class Action {
public:
  Action() = default;

  virtual std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept = 0;
  virtual ~Action() = default;

  virtual std::size_t length() const noexcept = 0;
};

template <uint32_t ROWS, uint32_t COLS>
class MovementAction : public Action<ROWS, COLS> {
public:
  enum direction { UP, DOWN, LEFT, RIGHT };

  MovementAction(const std::string& data) {
    switch (static_cast<uint8_t>(data[0])) {
      case (uint8_t)0: dir = direction::UP; break;
      case (uint8_t)1: dir = direction::DOWN; break;
      case (uint8_t)2: dir = direction::LEFT; break;
      case (uint8_t)3: dir = direction::RIGHT; break;
      default: dir = direction::DOWN; break;
    }
  }

  direction get_direction() const noexcept { return dir; }
  std::size_t length() const noexcept { return 1; }

  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    auto new_pos = pos;

    switch (dir) {
      case UP:    --new_pos.second; break;
      case DOWN:  ++new_pos.second; break;
      case LEFT:  --new_pos.first; break;
      case RIGHT: ++new_pos.first; break;
    }

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

  std::size_t length() const noexcept { return 9; }

private:
  uint32_t x;
  uint32_t y;

  bool is_valid = true;

  bool is_valid_payload(const std::string& data) {
    return data.size() >= length() && data[0] == 0x05;
  }

  void extract_position(const std::string& payload) {
    x = static_cast<uint32_t>(payload[4] << 24 | payload[3] << 16 | payload[2] << 8 | payload[1]);
    y = static_cast<uint32_t>(payload[8] << 24 | payload[7] << 16 | payload[6] << 8 | payload[5]);
  }
};

template<uint32_t ROWS, uint32_t COLS>
class PatternAction : public Action<ROWS, COLS> {
public:
  PatternAction(const std::string& data) {
    std::size_t count = 0;
    std::size_t offset = length();

    while (offset < data.size() || count < 5) {
      auto action = RetrieveAction<ROWS, COLS>{}(data.substr(offset));
      offset += action->length();
      actions.push_back(std::move(action));
      ++count;
    }
  }

  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    return pos;
  }

  std::vector<std::shared_ptr<Action<ROWS, COLS>>>& get_actions() noexcept { return actions; }

  std::size_t length() const noexcept { return 1; }

private:
  std::vector<std::shared_ptr<Action<ROWS, COLS>>> actions{ };
};

template <uint32_t ROWS, uint32_t COLS>
class UndefinedAction : public Action<ROWS, COLS> {
public:
  std::pair<uint32_t, uint32_t> perform(const std::pair<uint32_t, uint32_t>& pos) const noexcept {
    return pos;
  }

  std::size_t length() const noexcept { return 1; }
};


template <uint32_t ROWS, uint32_t COLS>
struct RetrieveAction {
  std::shared_ptr<Action<ROWS, COLS>> operator()(const std::string& data) const noexcept {
    if (data.empty()) 
      return std::make_shared<UndefinedAction<ROWS, COLS>>();

    if (data[0] <= 0x03)
      return std::make_shared<MovementAction<ROWS, COLS>>(data);

    if (data[0] == 0x05)
      return std::make_shared<TeleportAction<ROWS, COLS>>(data);

    if (data[0] == 0x07)
      return std::make_shared<PatternAction<ROWS, COLS>>(data);

    return std::make_shared<UndefinedAction<ROWS, COLS>>();
  }
};


#endif // SPACE_ACTION_H
