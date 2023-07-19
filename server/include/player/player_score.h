#ifndef SPACE_PLAYER_PLAYER_SCORE_H
#define SPACE_PLAYER_PLAYER_SCORE_H

#include "common.h"

#include <tuple>

class player_score {
public:
  uint64_t total_score;

  std::tuple<uint64_t, uint64_t, uint64_t> tick_score;

  explicit player_score(uint64_t score) 
    : total_score(score), tick_score(std::make_tuple(0, 0, 0)) {}

  uint64_t score() noexcept;

  void add_zone_score(uint64_t n_tiles) noexcept;

  void add_kill_score(uint64_t trail_size) noexcept;

  void add_capture_score(uint64_t trail_size) noexcept;

  friend std::ostream& operator<<(std::ostream& os, const player_score& ps) {
    os  << " { total_score: " 
        << ps.total_score << ", tick_score: { zone_score: "  << std::get<0>(ps.tick_score) 
                                        << ", kill_score: " << std::get<1>(ps.tick_score) 
                                        << ", capture_score: " << std::get<2>(ps.tick_score) << " } }";
    return os;
  }
};

#endif //SPACE_PLAYER_PLAYER_SCORE_H