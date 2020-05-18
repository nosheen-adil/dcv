#ifndef DCV_DETAIL_UTILS_HPP
#define DCV_DETAIL_UTILS_HPP

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <opencv4/opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

namespace dcv {
namespace detail {

enum class TimestampPrecision {
  ns, u, ms, s
};

namespace utils {

static const std::string localhost = "127.0.0.1";

std::string SocketAddress(std::string endpoint, std::string transport = "tcp");
std::string SocketAddress(std::string ip, uint16_t port, std::string transport = "tcp");
std::string SocketAddress(uint16_t port, std::string transport = "tcp");
std::string SocketAddress(YAML::Node &&config);

bool MatsEqual(const cv::Mat &lhs, const cv::Mat &rhs, const double percentError = 0.0);

std::chrono::time_point<std::chrono::system_clock> Timepoint(
  const std::string timestamp,
  const TimestampPrecision precision = TimestampPrecision::ms
);

std::chrono::time_point<std::chrono::system_clock> Timepoint(
  const uint64_t timestamp,
  const TimestampPrecision precision = TimestampPrecision::ms
);

std::string TimestampIso8601(
  const std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now(),
  const TimestampPrecision precision = TimestampPrecision::ms
);

std::string TimestampIso8601(
  const uint64_t timestamp,
  const TimestampPrecision precision = TimestampPrecision::ms
);

uint64_t TimestampUnix(
  const std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now(),
  const TimestampPrecision precision = TimestampPrecision::ms
);


uint64_t TimestampUnix(
  const std::string timestamp,
  const TimestampPrecision precision = TimestampPrecision::ms
);

} // namespace utils
} // namespace detail
} // namespace dcv

#endif // DCV_DETAIL_UTILS_HPP