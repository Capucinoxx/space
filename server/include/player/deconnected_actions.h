#ifndef SPACE_PLAYER_DECONNECTED_ACTIONS_H
#define SPACE_PLAYER_DECONNECTED_ACTIONS_H

#include "common.h"
#include "action.h"

#include <vector>
#include <memory>

class deconnected_actions {
private:
  std::vector<std::shared_ptr<Action<rows, cols>>> actions;
  std::size_t index = 0;

public:
  explicit deconnected_actions()
    : actions{ std::make_shared<MovementAction<rows, cols>>(std::to_string('\x01')) }, index{ 0 } {}

  explicit deconnected_actions(const std::vector<std::shared_ptr<Action<rows, cols>>>& actions)
    : actions{ actions }, index{ 0 } {}

  std::shared_ptr<Action<rows, cols>> next() noexcept;

  void update(const std::vector<std::shared_ptr<Action<rows, cols>>>& actions) noexcept;

  void reset() noexcept;
};

#endif //SPACE_PLAYER_DECONNECTED_ACTIONS_H