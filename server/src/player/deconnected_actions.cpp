#include "player/deconnected_actions.h"

std::shared_ptr<Action<rows, cols>> deconnected_actions::next() noexcept {
  return actions[index++ % actions.size()];
}

void deconnected_actions::update(const std::vector<std::shared_ptr<Action<rows, cols>>>& actions) noexcept {
  this->actions = actions;
}

void deconnected_actions::reset() noexcept {
  index = 0;
}