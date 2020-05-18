#ifndef DCV_INFLUXDB_POINT_HPP
#define DCV_INFLUXDB_POINT_HPP

#include <map>
#include <rapidjson/document.h>

#include "dcv/detail/error.hpp"
#include "dcv/detail/utils.hpp"
#include "dcv/detail/logging.hpp"

namespace dcv {
namespace influxdb {

inline static void EncodeField(std::stringstream &out, const std::string &in) {
  out << "\"" << in << "\"";
}

inline static void EncodeField(std::stringstream &out, const char* in) {
  out << "\"" << in << "\"";
}

inline static void EncodeField(std::stringstream &out, const bool &in) {
  out << in;
}

inline static void EncodeField(std::stringstream &out, const int &in) {
  out << in << "i";
}

inline static void EncodeField(std::stringstream &out, const unsigned long int &in) {
  out << in << "i";
}

inline static void EncodeField(std::stringstream &out, const double in) {
  out << in;
}

class Point {
 public:
  Point(const std::string measurement);

  void SetMeasurement(const std::string value);
  std::string GetMeasurement() const;
  void SetTag(const std::string name, const std::string value);
  Point&& Tag(const std::string name, const std::string value);
  std::string GetTag(const std::string name) const;
  template<typename DataType>
  void SetField(const std::string name, const DataType value);
  template<typename DataType>
  Point&& Field(const std::string name, const DataType value);
  template<typename DataType>
  DataType GetField(const std::string name) const;
  void SetTime(const std::string value);
  void SetTime(const uint64_t time);
  Point&& Time(const std::string value);
  Point&& Time(const uint64_t value);
  std::string GetTime() const;
  std::string AsLine() const;

 private:
  std::string measurement;
  std::map<std::string, std::string> tagSet;
  std::map<std::string, std::string> fieldSet;
  std::string time;
};

template<typename DataType>
void Point::SetField(const std::string name, const DataType value) {
  std::stringstream ss;
  EncodeField(ss, value);
  fieldSet[name] = ss.str(); 
}

template<typename DataType>
Point&& Point::Field(const std::string name, const DataType value) {
  SetField(name, value);
  return std::move(*this);
}

template<typename DataType>
DataType Point::GetField(const std::string name) const {
  try {
    DataType data;
    auto value = fieldSet.at(name);
    if (value.back() == '"') {
      value = value.substr(1, value.length() - 2);
    }
    std::stringstream ss(value);
    ss >> data;
    return data;
  } catch (...) {
    throw detail::InfluxError("Point", "GetField", "Could not map \"" + name + "\" key to any value in point");
  }
}

inline static void FromJson(const rapidjson::Value &json, std::vector<Point> &points) {
  points.clear();
  if (!json.HasMember("series"))
    return;

  auto series = json["series"].GetArray();
  for (auto &_point : series) {
    Point base(_point["name"].GetString());
    if (_point.HasMember("tags")) {
      auto tags = _point["tags"].GetObject();
      for (auto &el : tags) {
        base.SetTag(el.name.GetString(), el.value.GetString());
      }
    }

    auto columns = _point["columns"].GetArray();
    auto values = _point["values"].GetArray();
    auto size = columns.Size();

    for (auto &v : values) {
      Point point = base;
      for (auto i = 0; i < size; i++) {
        auto field = columns[i].GetString();
        if (columns[i] == "time") {
          point.SetTime(v[i].GetString());
        } else if (v[i].IsString()) {
          point.SetField(field, v[i].GetString());
        } else if (v[i].IsBool()) {
          point.SetField(field, v[i].GetBool());
        } else if (v[i].IsDouble()) {
          point.SetField(field, v[i].GetDouble());
        } else if (v[i].IsUint64()) {
          point.SetField(field, v[i].GetUint64());
        } else if (v[i].IsInt()) {
          point.SetField(field, v[i].GetInt());
        }

      }
      points.push_back(std::move(point));
    }
  }
}

} // namesapce influxdb
} // namespace dcv

#endif // DCV_INFLUXDB_POINT_HPP