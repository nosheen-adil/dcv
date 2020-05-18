#include "dcv/influxdb/point.hpp"

namespace dcv {
namespace influxdb {

Point::Point(const std::string measurement):
  measurement(measurement) {
}

void Point::SetMeasurement(const std::string value) {
  measurement = value;
}

std::string Point::GetMeasurement() const {
  return measurement;
}

void Point::SetTag(const std::string name, const std::string value) {
  tagSet[name] = value;
}

Point&& Point::Tag(const std::string name, const std::string value) {
  SetTag(name, value);
  return std::move(*this);
}

std::string Point::GetTag(const std::string name) const {
  try {
    return tagSet.at(name);
  } catch (...) {
    throw detail::InfluxError("Point", "GetTag", "Could not map \"" + name + "\" key to any value in point");
  }
}

void Point::SetTime(const std::string value) {
  time = value;
}

void Point::SetTime(const uint64_t value) {
  time = detail::utils::TimestampIso8601(value);
}

Point&& Point::Time(const std::string value) {
  SetTime(value);
  return std::move(*this);
}

Point&& Point::Time(const uint64_t value) {
  SetTime(value);
  return std::move(*this);
}

std::string Point::GetTime() const {
  return time;
}

std::string Point::AsLine() const {
  std::string _tagSet;
  for (auto kv : tagSet) {
    _tagSet += ",";
    _tagSet += kv.first;
    _tagSet += "=";
    _tagSet += kv.second;
  }
  
  std::string _fieldSet;
  bool first = true;
  for (auto kv : fieldSet) {
    if (!first) _fieldSet += ",";
    _fieldSet += kv.first;
    _fieldSet += "=";
    _fieldSet += kv.second;
    first = false;
  }

  auto line = measurement + _tagSet + " " + _fieldSet;
  if (!time.empty()) {
    line += " ";
    line += std::to_string(detail::utils::TimestampUnix(time));
  }
  return line;
}

} // namespace influxdb
} // namespace dcv