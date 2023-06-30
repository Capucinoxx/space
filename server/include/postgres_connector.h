#ifndef SPACE_POSTGRES_H
#define SPACE_POSTGRES_H

#include "configuration.h"
#include <pqxx/pqxx>

#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <string>

class PostgresConnector {
public:
  PostgresConnector(const PostgresConnector&) = delete;
  PostgresConnector& operator=(const PostgresConnector&) = delete;

  static std::shared_ptr<PostgresConnector>& get_instance(const Config& cfg) {
    static std::shared_ptr<PostgresConnector> singleton(new PostgresConnector(cfg));
    return singleton;
  }

private:
  static std::shared_ptr<PostgresConnector> singleton;
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


  template<typename T>
  pqxx::result bulk_insert(const std::string& query, const std::vector<T>& params) {
    try {
      pqxx::work w(*connection);

      
      pqxx::result result = w.exec_params(query, pqxx::prepare::make_dynamic_params(params));
      w.commit();

      return result;
    } catch(const std::exception &e) {
      return pqxx::result();
    }
  }


  template<typename... Args>
  pqxx::result execute(const std::string& query, Args&&... args) {    
    try {
      pqxx::work w(*connection);

      pqxx::result result = w.exec_params(query, std::forward<Args>(args)...);
      w.commit();

      return result;
    } catch(const std::exception &e) {
      return pqxx::result();
    }
  }
 
  bool connected() const noexcept {
    return connection->is_open();
  }
};

#endif //SPACE_POSTGRES_H