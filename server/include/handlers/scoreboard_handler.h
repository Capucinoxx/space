#ifndef SPACE_HANDLERS_SCOREBOARD_HANDLER_H
#define SPACE_HANDLERS_SCOREBOARD_HANDLER_H

#include "network.h"
#include "postgres_connector.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>


template<uint32_t ROWS, uint32_t COLS>
class ScoreboardHandle {
public:
  using psql_ref = std::shared_ptr<PostgresConnector>;

private:
  psql_ref postgres;

public:
  explicit ScoreboardHandle(psql_ref postgres) 
    : postgres(postgres) {}

  void operator()(Server& server) {
    server.add_http_endpoint("/scoreboard", [&](Server::http_request req) -> std::pair<http::status, std::string> {
      return handle_scoreboard(req);
    });
  }

private:
  std::pair<http::status, std::string> handle_scoreboard(Server::http_request req) {
    std::string query = R"(WITH ranked_scores AS (
      SELECT player_id, timestamp, score,
      ROW_NUMBER() OVER (PARTITION BY player_id, date_trunc('minute', timestamp) ORDER BY timestamp) AS row_num
      FROM player_scores
      ) SELECT p.name, p.h, p.s, p.l, rs.timestamp, rs.score FROM ranked_scores rs
      JOIN player p ON rs.player_id = p.id
      WHERE rs.row_num = 1)";

    auto result = postgres->execute(query);
    

    if (result.empty())
      return std::make_pair(http::status::internal_server_error, "failed to retrieve scoreboard");

    std::unordered_map<std::string, std::vector<uint8_t>> scores;
    std::unordered_map<std::string, std::tuple<boost::float64_t, boost::float64_t, boost::float64_t>> player_info;

    for (auto row : result) {
      std::string player_name = row[0].as<std::string>();
      if (scores.find(player_name) == scores.end()) {
        scores[player_name] = std::vector<uint8_t>();
        player_info[player_name] = std::make_tuple(row[1].as<boost::float64_t>(), row[2].as<boost::float64_t>(), row[3].as<boost::float64_t>());
      }
        

      uint64_t epoch = parse_string_to_epoch(row[4].as<std::string>());
      boost::float64_t score = row[5].as<boost::float64_t>();

      serialize_value<uint64_t>(scores[player_name], epoch);
      serialize_value<boost::float64_t>(scores[player_name], score);
    }

    std::vector<uint8_t> serialized_scores;
    for (auto [name, serialized_score] : scores) {
      // name
      serialize_value<uint32_t>(serialized_scores, name.size());
      serialize_data<std::string>(serialized_scores, name, name.size());

      // color
      auto [h, s, l] = player_info[name];
      serialize_value<boost::float64_t>(serialized_scores, h);
      serialize_value<boost::float64_t>(serialized_scores, s);
      serialize_value<boost::float64_t>(serialized_scores, l);

      // each score entry is 8bytes for epoch and 8bytes for score
      serialize_value<uint32_t>(serialized_scores, serialized_score.size() / 16);

      // scores
      serialized_scores.insert(serialized_scores.end(), serialized_score.begin(), serialized_score.end());
    }

    return std::make_pair(http::status::ok, std::string(serialized_scores.begin(), serialized_scores.end()));
  }
};

#endif  // SPACE_HANDLERS_SCOREBOARD_HANDLER_H