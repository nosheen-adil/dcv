#ifndef DCV_INFLUXDB_CLIENT_HPP
#define DCV_INFLUXDB_CLIENT_HPP

#include <thread>
#include <zmq.hpp>

#include "dcv/influxdb/database_config.hpp"
#include "dcv/influxdb/point.hpp"
#include "dcv/detail/logging.hpp"
#include "dcv/detail/http.hpp"

namespace dcv {
namespace influxdb {

class Client {
 public:
  Client(const DatabaseConfig databaseConfig = DatabaseConfig());
  ~Client();
  
  void SetDatabase(const DatabaseConfig value);
  DatabaseConfig GetDatabase() const;
  void Query(const std::string query);
  void Query(const std::string query, std::string &response);
  void Query(const std::string query, rapidjson::Value &result);
  void Query(const std::string query, std::vector<Point> &points);
  void Write(const Point &&point);
  void Write(std::vector<std::string> &batch);

 private:
  CURL *writeHandle;
  CURL *queryHandle;
  DatabaseConfig databaseConfig;
};

} // namespace influxdb
} // namespace dcv

#endif // DCV_INFLUXDB_CLIENT_HPP