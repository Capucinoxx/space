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

  template<typename... Args>
  pqxx::result execute_query(const std::string& query, Args&&... args) {
    try {
      pqxx::work w(*connection);

      pqxx::prepare::invocation invoc = w.prepared("custom_query");
      set_parameters(invoc, 1, std::forward<Args>(args)...);

      pqxx::result result = invoc.exec(query);

      w.commit();
      return result;
    } catch(const std::exception &e) {
      return pxqq::result();
    }

    pqxx::result result = w.exec(query);

    w.exec(query);
    w.commit();

    return result;
  }
 
  bool connected() const noexcept {
    return connection->is_open();
  }

private:
  void set_parameters(pqxx::prepare::invocation& invoc) {};

  template<typename T, typename... Args>
  void set_parameters(pqxx::prepare::invocation& invoc, int index, const T& value, Args&&... args) {
    invoc("$" + std::to_string(index), value);
    set_parameters(invoc, index + 1, std::forward<Args>(args)...);
  }
};

#endif //SPACE_POSTGRES_H