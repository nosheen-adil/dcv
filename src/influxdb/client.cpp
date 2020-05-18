#include "dcv/influxdb/client.hpp"77

namespace dcv {
namespace influxdb {

Client::Client(
  const DatabaseConfig databaseConfig
  ):
  databaseConfig(databaseConfig) {
  
  writeHandle = curl_easy_init();
  if (!writeHandle)
    throw detail::InfluxError("Client", "Client", "Could not create writeHandle");

  queryHandle = curl_easy_init();
  if (!queryHandle)
    throw detail::InfluxError("Client", "Client", "Could not create queryHandle");

  std::string writeUrl = databaseConfig.Url("write");
  curl_easy_setopt(writeHandle, CURLOPT_URL, writeUrl.c_str());

  detail::http::InitPostHandle(writeHandle);
  detail::http::InitGetHandle(queryHandle);

  for (auto i = 0; i < databaseConfig.connectionRetries; i++) {
    if (curl_easy_perform(writeHandle) == CURLE_OK)
      return;
    logging::warn("[dcv::influxdb::Client::Client()] - Connection failed. Retrying after {} ms", databaseConfig.connectionTimeout);
    std::this_thread::sleep_for(std::chrono::milliseconds(databaseConfig.connectionTimeout));
  }
  throw detail::InfluxError("Client", "Client", "Could not connect to database server");
}

Client::~Client() {
  curl_easy_cleanup(writeHandle);
  curl_easy_cleanup(queryHandle);
}

void Client::SetDatabase(const DatabaseConfig value) {
  databaseConfig = value;
}

DatabaseConfig Client::GetDatabase() const {
  return databaseConfig;
}

void Client::Query(const std::string query) {
  std::string response;
  std::string url = databaseConfig.Url("query") + std::string("&q=") + curl_easy_escape(queryHandle, query.c_str(), query.size());
  curl_easy_setopt(queryHandle, CURLOPT_URL, url.c_str());
  detail::http::Get(queryHandle, response);
}

void Client::Query(const std::string query, std::string &response) {
  std::string url = databaseConfig.Url("query") + std::string("&q=") + curl_easy_escape(queryHandle, query.c_str(), query.size());
  curl_easy_setopt(queryHandle, CURLOPT_URL, url.c_str());
  detail::http::Get(queryHandle, response);
}

void Client::Query(const std::string query, rapidjson::Value &result) {
  rapidjson::Document response;
  std::string _response;
  Query(query, _response);
  const char *r = _response.c_str();
  response.Parse(r);
  auto results = response["results"].GetArray();
  result = results[0].GetObject();
}

void Client::Query(const std::string query, std::vector<Point> &points) {
  rapidjson::Value result;
  Query(query, result);
  FromJson(result, points);
}

void Client::Write(const Point &&point) {
  const std::string line = point.AsLine();
  detail::http::Post(writeHandle, line);
}

void Client::Write(std::vector<std::string> &batch) {
  std::string body;
  for (auto &line : batch) {
    body += line;
    body += "\n";
  }
  detail::http::Post(writeHandle, body);
}

} // namespace influxdb
} // namespace dcv