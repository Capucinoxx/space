#ifndef SPACE_POSTGRES_H
#define SPACE_POSTGRES_H

#include "configuration.h"
#include <pqxx/pqxx>
#include <sstream>

class PostgresConnector {
public:
  PostgresConnector(const PostgresConnector&) = delete;
  PostgresConnector& operator=(const PostgresConnector&) = delete;

  static PostgresConnector& get_instance(const Config& cfg) {
    static PostgresConnector singleton(cfg);
    return singleton;
  }

private:
  static PostgresConnector singleton;
  std::unique_ptr<pqxx::connection> connection;

  PostgresConnector(const Config& cfg) {
    std::string host = cfg.at("HOST");
    std::string port = cfg.at("PORT");
    std::string user = cfg.at("USER");
    std::string password = cfg.at("PASSWORD");
    std::string dbname = cfg.at("DATABASE");

    std::stringstream ss;
    ss << "postgresql://" << user 
       << ":" << password 
       << "@" << host 
       << ":" << port 
       << "/" << dbname;

    connection = std::make_unique<pqxx::connection>(ss.str());
  }

public:
  pqxx::result execute_query(const std::string& query) {
    pqxx::work w(*connection);

    pqxx::result result = w.exec(query);

    w.exec(query);
    w.commit();

    return result;
  }
 
  bool connected() const noexcept {
    return connection->is_open();
  }
};

#endif //SPACE_POSTGRES_H