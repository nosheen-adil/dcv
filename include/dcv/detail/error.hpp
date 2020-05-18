#ifndef DCV_DETAIL_ERROR_HPP
#define DCV_DETAIL_ERROR_HPP

#include <exception>

namespace dcv {
namespace detail {

class Error : public std::runtime_error {
 public:
  Error(const std::string c, const std::string f, const std::string w):
    std::runtime_error("Exception thrown from dcv::" + c + "::" + f + "():\n" + w) {
  }
};

class StopError : public Error {
 public:
  StopError(const std::string c, const std::string f, const std::string w):
    Error(c, f, "Instance has stopped execution - \"" + w + "\"") {
  }
};

class InfluxError : public Error {
 public:
  InfluxError(const std::string c, const std::string f, const int rc, const std::string response):
    Error("influxdb::" + c, f,  "HTTP Response Code " + std::to_string(rc) + "\n" + response) {
  }

  InfluxError(const std::string c, const std::string f, const std::string w):
    Error("influxdb::" + c, f,  w) {
  }
};

class HttpError : public Error {
 public:
  HttpError(const std::string f, const long rc):
    Error("http", f, "Response Code: " + std::to_string(rc)) {
  }

  HttpError(const std::string f, const std::string &e):
    Error("http", f, "Curl Error " + e) {
  }
};

} // namespace detail
} // namespace dcv

#endif // DCV_DETAIL_ERROR_HPP