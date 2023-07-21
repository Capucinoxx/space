#ifndef SPACE_HANDLER_SCOREBOARD_HANDLER_H
#define SPACE_HANDLER_SCOREBOARD_HANDLER_H

#include "handler/handler.h"
#include "postgres_connector.h"

#include <unordered_map>
#include <vector>
#include <boost/cstdfloat.hpp> 

class scoreboard_handler : public handler {
private:
  static constexpr const char * query = R"(WITH ranked_scores AS (
      SELECT player_id, timestamp, score,
      ROW_NUMBER() OVER (PARTITION BY player_id, date_trunc('minute', timestamp) ORDER BY timestamp) AS row_num
      FROM player_scores
      ) SELECT p.name, p.h, p.s, p.l, rs.timestamp, rs.score FROM ranked_scores rs
      JOIN player p ON rs.player_id = p.id
      WHERE rs.row_num = 1
      AND EXTRACT('minute' FROM rs.timestamp) % 3 = 0
      ORDER BY rs.player_id)";

  psql_sptr postgres;

  using hsl_color = std::tuple<boost::float64_t, boost::float64_t, boost::float64_t>;

public:
  scoreboard_handler(psql_sptr postgres) : postgres(postgres) {};
  ~scoreboard_handler() {};

  void operator()(http::request<http::string_body>& req, http::response<http::string_body>& resp) override;

private:
  std::pair<http::status, std::string> handle();
};

#endif //SPACE_HANDLER_SCOREBOARD_HANDLER_H