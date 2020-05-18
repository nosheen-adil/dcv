#include "dcv/detail/utils.hpp"

namespace dcv {
namespace detail {
namespace utils {

std::string SocketAddress(std::string endpoint, std::string transport) {
  return transport + "://" + endpoint;
}

std::string SocketAddress(std::string ip, uint16_t port, std::string transport) {
  return SocketAddress(ip + ":" + std::to_string(port), transport);
}

std::string SocketAddress(uint16_t port, std::string transport) {
  return SocketAddress("*:" + std::to_string(port), transport);
}

std::string SocketAddress(YAML::Node &&config) {
  if (config.Type() == YAML::NodeType::Scalar)
    return config.as<std::string>();
  
  std::string transport = "tcp";
  if (config["transport"])
    transport = config["transport"].as<std::string>();
  if (config["port"]) {
    if (config["ip"])
      return SocketAddress(config["ip"].as<std::string>(), config["port"].as<uint16_t>(), transport);
    return SocketAddress(config["port"].as<uint16_t>(), transport);
  }
  return SocketAddress(config["endpoint"].as<std::string>(), transport);
}

std::chrono::time_point<std::chrono::system_clock> Timepoint(
  const std::string timestamp,
  const TimestampPrecision precision
) {
  std::string _timestamp = timestamp;
  if (_timestamp.back() == 'Z')
    _timestamp = _timestamp.substr(0, _timestamp.length() - 1);

  int p = 0;
  int len = 0;
  auto pos = _timestamp.find(".");
  if (pos != std::string::npos) {
    len = _timestamp.length() - pos - 1;
    p = atoi(_timestamp.substr(pos + 1).c_str());
    _timestamp = _timestamp.substr(0, pos);
  }

  std::tm tm = {};
  std::istringstream ss(_timestamp);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  auto t = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  if (len == 3) {
    t += std::chrono::milliseconds(p);
  } else if (len == 6) {
    t += std::chrono::microseconds(p);
  } else if (len == 9) {
    t += std::chrono::nanoseconds(p);
  }
  return t;
}

std::chrono::time_point<std::chrono::system_clock> Timepoint(
  const uint64_t timestamp,
  const TimestampPrecision precision
) {
  auto epoch = std::chrono::time_point<std::chrono::system_clock>();
  if (precision == TimestampPrecision::s) {
    return epoch + std::chrono::seconds(timestamp);
  } else if (precision == TimestampPrecision::ms) {
    return epoch + std::chrono::milliseconds(timestamp);
  } else if (precision == TimestampPrecision::u) {
    return epoch + std::chrono::microseconds(timestamp);
  } else {
    return epoch + std::chrono::nanoseconds(timestamp);
  }
}

std::string TimestampIso8601(
  const std::chrono::time_point<std::chrono::system_clock> t,
  const TimestampPrecision precision
) {
  std::string timestamp;
  std::stringstream ss;

  auto seconds = std::chrono::system_clock::to_time_t(t);
  ss << std::put_time(gmtime(&seconds), "%FT%T");

  auto truncated = std::chrono::system_clock::from_time_t(seconds);
  if (precision == TimestampPrecision::ms) {
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(t - truncated).count();
    ss << "." << std::fixed << std::setw(3) << std::setfill('0') << delta;
  } else if (precision == TimestampPrecision::u) {
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(t - truncated).count();
    ss << "." << std::fixed << std::setw(6) << std::setfill('0') << delta;
  } else if (precision == TimestampPrecision::ns) {
    auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(t - truncated).count();
    ss << "." << std::fixed << std::setw(9) << std::setfill('0') << delta;
  }

  ss << "Z";
  return ss.str();
}

std::string TimestampIso8601(
  const uint64_t timestamp,
  const TimestampPrecision precision
) {
  auto t = Timepoint(timestamp, precision);
  return TimestampIso8601(t, precision);
}

uint64_t TimestampUnix(
  const std::chrono::time_point<std::chrono::system_clock> t,
  const TimestampPrecision precision
) {
  auto duration = t.time_since_epoch();
  if (precision == TimestampPrecision::s) {
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  } else if (precision == TimestampPrecision::ms) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  } else if (precision == TimestampPrecision::u) {
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  } else {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  }
}

uint64_t TimestampUnix(
  const std::string timestamp,
  const TimestampPrecision precision
) {
  auto t = Timepoint(timestamp, precision);
  return TimestampUnix(t, precision);
}

bool MatsEqual(const cv::Mat &lhs, const cv::Mat &rhs, const double percentError) {
  const auto numChannels = lhs.channels();
  assert(numChannels == rhs.channels());

  cv::Mat dst;
  std::vector<cv::Mat> channels(numChannels);
  cv::bitwise_xor(lhs, rhs, dst);
  cv::split(dst, channels);
  const auto numPixels = dst.rows * dst.cols;

  for (auto i = 0; i < numChannels; i++) {
    if (static_cast<float>(cv::countNonZero(channels[i]) / numPixels) > percentError)
      return false;
  }

  return true;
}

} // namespace detail
} // namespace utils
} // namespace dcv