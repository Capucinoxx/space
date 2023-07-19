#include "player/player_score.h"

uint64_t player_score::score() noexcept {
  total_score += std::get<0>(tick_score) + std::get<1>(tick_score) + std::get<2>(tick_score);
  tick_score = std::make_tuple(0, 0, 0);
  return total_score;
}

void player_score::add_zone_score(uint64_t n_tiles) noexcept {
  std::get<0>(tick_score) += n_tiles;
}

void player_score::add_kill_score(uint64_t trail_size) noexcept {
  std::get<1>(tick_score) += (12 * (1 + trail_size));
}

void player_score::add_capture_score(uint64_t trail_size) noexcept {
  std::get<2>(tick_score) += (3 * (1 + trail_size));
}