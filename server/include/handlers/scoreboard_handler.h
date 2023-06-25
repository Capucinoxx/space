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
  using game_ptr = std::shared_ptr<GameManager<ROWS, COLS>>;
  using psql_ref = PostgresConnector&;

private:
  game_ptr game;
  psql_ref postgres;

public:
  ScoreboardHandle(game_ptr game, psql_ref postgres) 
    : game(game), postgres(postgres) {}

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
      ) SELECT player_id, timestamp, score FROM ranked_scores 
      WHERE row_num = 1)";

    auto result = postgres.execute(query);
    

    if (result.empty())
      return std::make_pair(http::status::internal_server_error, "failed to retrieve scoreboard");

    std::unordered_map<std::string, std::vector<uint8_t>> scores;

    for (auto row : result) {
      std::string player_name = row[0].as<std::string>();
      if (scores.find(player_name) == scores.end())
        scores[player_name] = std::vector<uint8_t>();

      uint64_t epoch = parse_string_to_epoch(row[1].as<std::string>());
      boost::float64_t score = row[2].as<boost::float64_t>();

      serialize_value<uint64_t>(scores[player_name], epoch);
      serialize_value<boost::float64_t>(scores[player_name], score);
    }

    std::vector<uint8_t> serialized_scores;
    for (auto [name, serialized_score] : scores) {
      serialize_data<std::string>(serialized_scores, name, name.size());
      serialized_scores.insert(serialized_scores.end(), serialized_score.begin(), serialized_score.end());
      serialize_value<uint64_t>(serialized_scores, 0);
      serialize_value<boost::float64_t>(serialized_scores, 0.0);
    }

    return std::make_pair(http::status::ok, std::string(serialized_scores.begin(), serialized_scores.end()));
  }
};

#endif  // SPACE_HANDLERS_SCOREBOARD_HANDLER_H