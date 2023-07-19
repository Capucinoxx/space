#ifndef SPACE_PLAYER_TELEPORT_STMT_H
#define SPACE_PLAYER_TELEPORT_STMT_H

#include <iostream>

class teleport_stmt {
private:
  constexpr static uint8_t TTL = 8;
  uint8_t ttl = 0;

public:
  teleport_stmt() noexcept = default;

  bool can_teleport() const noexcept {
    return ttl == 0;
  }

  void tick() noexcept {
    if (ttl > 0)
      --ttl;
  }

  void teleport() noexcept {
    ttl = TTL;
  }
};

#endif //SPACE_PLAYER_TELEPORT_STMT_H