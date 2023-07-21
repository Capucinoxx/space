#include "handler/scoreboard_handler.h"
#include "common.h"


#include <iostream>
#include <string>
#include <sstream>
#include <tuple>

void scoreboard_handler::operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) {
  auto [status, body] = handle();
  resp.result(status);
  resp.body() = std::move(body);
  resp.keep_alive(req.keep_alive());
  resp.set(http::field::server, "Space");
  resp.set(http::field::content_type, "application/octet-stream");
  resp.set(http::field::access_control_allow_origin, "*");
  resp.prepare_payload();
}

std::pair<http::status, std::string> scoreboard_handler::handle() {
  auto result = postgres->execute(query);
  if (result.empty())
    return {http::status::bad_request, "no scores found"};

  std::vector<uint8_t> buffer;
  std::string current_player;

  std::vector<uint8_t> scores;

  for (const auto& row : result) {
    std::string player_name = row[0].as<std::string>();
    if (player_name != current_player) {
      if (!current_player.empty()) {
        serialize_value<uint32_t>(buffer, scores.size() / 16);
        buffer.insert(buffer.end(), scores.begin(), scores.end());
        scores.clear();
      }
      
      serialize_value<uint32_t>(buffer, player_name.size());
      serialize_data<std::string>(buffer, player_name, player_name.size());
      serialize_value<boost::float64_t>(buffer, row[1].as<boost::float64_t>());
      serialize_value<boost::float64_t>(buffer, row[2].as<boost::float64_t>());
      serialize_value<boost::float64_t>(buffer, row[3].as<boost::float64_t>());
      current_player = player_name;
    }

    uint64_t epoch = parse_string_to_epoch(row[4].as<std::string>());
    auto score = row[5].as<boost::float64_t>();

    serialize_value<uint64_t>(scores, epoch);
    serialize_value<boost::float64_t>(scores, score);
  }

  serialize_value<uint32_t>(buffer, scores.size() / 16);
  buffer.insert(buffer.end(), scores.begin(), scores.end());

  return {http::status::ok, std::move(std::string(buffer.begin(), buffer.end()))};
}

