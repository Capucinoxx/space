#include "handler/game_handler.h"

#include <boost/format.hpp>
#include <boost/cstdfloat.hpp> 
#include <iostream>

void game_handler::operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) {}

bool game_handler::on_open(http::request<http::string_body>& req) {
  std::string token = req[http::field::authorization].to_string();
  if (token.empty())
    return false;

  auto result = postgres->execute(boost::str(boost::format(query) % token));
  if (result.size() != 1)
    return false;

  uint32_t id = result[0][0].as<uint32_t>(0);
  std::string name = result[0][1].as<std::string>("");

  boost::float64_t h = result[0][2].as<boost::float64_t>(0.0);
  boost::float64_t s = result[0][3].as<boost::float64_t>(0.0);
  boost::float64_t l = result[0][4].as<boost::float64_t>(0.0);

  uint64_t score = result[0][5].as<uint64_t>(0);

  player = game_state->register_player(name, id, { h, s, l }, score);
  return player != nullptr;
}

void game_handler::on_close() {
  if (player != nullptr)
    game_state->disconnect_player(player->id());
}

void game_handler::on_message(std::string message) {
  game_state->push({ player, std::move(message) });
}
