#ifndef SPACE_POSTGRES_H
#define SPACE_POSTGRES_H

#include <pqxx/pqxx>

class PostgresConnector {
public:
  PostgresConnector(const PostgresConnector&) = delete;
  PostgresConnector& operator=(const PostgresConnector&) = delete;

private:
  static PostgresConnector singleton;
  std::unique_ptr<pqxx::connection> connection;

  PostgresConnector(const std::string& conn_str) {
    connection = std::make_unique<pqxx::connection>(conn_str);
  }

  pqxx::result execute_query(const std::string& query) {
    pqxx::work w(*connection);

    pqxx::result result = w.exec(query);

    w.exec(query);
    w.commit();

    return result;
  }
};

#endif //SPACE_POSTGRES_H