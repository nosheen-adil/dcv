#ifndef DCV_INFLUXDB_CONFIG_HPP
#define DCV_INFLUXDB_CONFIG_HPP

#include "rapidjson/document.h"

#include "dcv/detail/utils.hpp"

namespace dcv {
namespace influxdb {

struct DatabaseConfig {
  std::string address;
  std::string database;
  std::string user;
  std::string password;
  std::string precision;
  uint64_t connectionTimeout; // ms
  uint64_t connectionRetries;

  DatabaseConfig(
    std::string ip = "localhost",
    const uint16_t port = 8086,
    std::string database = "dcv",
    std::string user = "",
    std::string password = "",
    std::string precision = "ms",
    uint64_t connectionTimeout = 1000,
    uint64_t connectionRetries = 20
  ):
    address(detail::utils::SocketAddress(ip, port, "http")),
    database(database), user(user), password(password), precision(precision),
    connectionTimeout(connectionTimeout), connectionRetries(connectionRetries) {
  }

  std::string Url(const std::string endpoint) const {
    std::stringstream url;
    bool a = false;
    url << address << "/" << endpoint << "?";
    if (!database.empty())
      url << "db=" << database << "&";
    if (!user.empty())
      url << "u=" << user << "&";
    if (!password.empty())
      url << "p=" << password << "&";
    url << "precision=" << precision;
    return url.str();
  }

  static DatabaseConfig FromYaml(YAML::Node &&config) {
    std::string ip = detail::utils::localhost;
    uint16_t port = 8086;
    std::string database = "dcv";
    std::string user = "";
    std::string password = "";
    std::string precision = "ms";
    uint64_t connectionTimeout = 1000;
    uint64_t connectionRetries = 20;

    if (config["ip"])
      ip = config["ip"].as<std::string>();
    if (config["port"])
      port = config["port"].as<uint16_t>();
    if (config["database"])
      database = config["database"].as<std::string>();
    if (config["user"])
      user = config["user"].as<std::string>();
    if (config["password"])
      password = config["password"].as<std::string>();
    if (config["precision"])
      precision = config["precision"].as<std::string>();
    if (config["connectionTimeout"])
      connectionTimeout = config["connectionTimeout"].as<uint64_t>();
    if (config["connectionRetries"])
      connectionRetries = config["connectionRetries"].as<uint64_t>();

    return DatabaseConfig(
      ip, port,
      database, user, password, precision,
      connectionTimeout, connectionRetries
    );
  }
};

} // namespace influxdb
} // namespace dcv

#endif // DCV_INFLUXDB_CONFIG_HPP