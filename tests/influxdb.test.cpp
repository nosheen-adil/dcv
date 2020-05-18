#include <thread>
#include <gtest/gtest.h>
#include <dcv/influxdb/client.hpp>

using namespace dcv;

TEST(PointTest, GetSetMeasurement) {
  auto p = influxdb::Point("something");
  p.SetMeasurement("else");
  ASSERT_EQ(p.GetMeasurement(), "else");
}

TEST(PointTest, GetInvalidTag) {
  auto p = influxdb::Point("node");
  p.SetTag("name", "yogurt");
  try {
    p.GetTag("nom");
    FAIL() << "expected dcv::detail::InfluxError";
  } catch (detail::InfluxError const &error) {
    ASSERT_EQ(
      error.what(),
      std::string("Exception thrown from dcv::influxdb::Point::GetTag():\nCould not map \"nom\" key to any value in point")
    );
  } catch (...) {
    FAIL() << "expected dcv::detail::InfluxError";
  }
}

TEST(PointTest, GetSetTag) {
  auto p = influxdb::Point("node");
  p.SetTag("name", "yogurt");
  ASSERT_EQ(p.GetTag("name"), "yogurt");
}

TEST(PointTest, GetInvalidField) {
  auto p = influxdb::Point("node");
  p.SetField("name", "yogurt");
  try {
    p.GetField<std::string>("nom");
    FAIL() << "expected dcv::detail::InfluxError";
  } catch (detail::InfluxError const &error) {
    ASSERT_EQ(
      error.what(),
      std::string("Exception thrown from dcv::influxdb::Point::GetField():\nCould not map \"nom\" key to any value in point"));
  } catch (...) {
    FAIL() << "expected dcv::detail::InfluxError";
  }
}

TEST(PointTest, GetSetStringField) {
  auto p = influxdb::Point("node");
  p.SetField("name", "yogurt");
  ASSERT_EQ(p.GetField<std::string>("name"), "yogurt");
}

TEST(PointTest, GetSetBooleanField) {
  auto p = influxdb::Point("node");
  p.SetField("name", false);
  ASSERT_EQ(p.GetField<bool>("name"), false);
}

TEST(PointTest, GetSetIntegerField) {
  auto p = influxdb::Point("node");
  p.SetField("name", 309);
  ASSERT_EQ(p.GetField<int>("name"), 309);
}

TEST(PointTest, GetSetDoubleField) {
  auto p = influxdb::Point("node");
  p.SetField("name", 3.33);
  ASSERT_EQ(p.GetField<double>("name"), 3.33);
}

TEST(PointTest, GetSetTimestampUnix) {
  auto p = influxdb::Point("node");
  p.SetTime(711634689123);
  ASSERT_EQ(p.GetTime(), "1992-07-20T12:18:09.123Z");
}

TEST(PointTest, GetSetTimestampISO8601) {
  auto p = influxdb::Point("node");
  p.SetTime("2020-04-08T21:57:04.014Z");
  ASSERT_EQ(p.GetTime(), "2020-04-08T21:57:04.014Z");
}

TEST(PointTest, AsLine) {
  std::string line = "node,machine=venus,name=VideoSource active=1,cpu=0.879256,fps=25i,source=\"rtsp://172.0.0.1\" 1586472382550";

  auto p = influxdb::Point("node")
    .Tag("name", "VideoSource")
    .Field("source", "rtsp://172.0.0.1")
    .Tag("machine", "venus")
    .Field("fps", 25)
    .Field("cpu", 0.879256)
    .Field("active", true)
    .Time(1586472382550);

  ASSERT_EQ(p.AsLine(), line);
}

TEST(PointTest, FromJson) {
  std::string result = R"(
    {
      "statement_id": 0,
      "series": [
        {
          "name": "node",
          "tags": {
            "name": "VideoSource"
          },
          "columns": [
            "time",
            "cpu"
          ],
          "values": [
            [
              "2020-04-08T21:57:04.014Z",
              0.897657
            ],
            [
              "2020-04-08T21:57:03.768Z",
              0.982738
            ]
          ]
        },
        {
          "name": "node",
          "tags": {
            "name": "InferenceFilter"
          },
          "columns": [
            "time",
            "fps",
            "active",
            "source"
          ],
          "values": [
            [
              "2020-04-08T21:57:04.073Z",
              10,
              true,
              "something"
            ]
          ]
        }
      ]
    }
  )";

  rapidjson::Document d;
  rapidjson::Value v;
  std::vector<influxdb::Point> points;

  d.Parse(result.c_str());
  v = d.GetObject();
  FromJson(v, points);
  ASSERT_EQ(points.size(), 3);

  ASSERT_EQ(points[0].GetMeasurement(), "node");
  ASSERT_EQ(points[0].GetTag("name"), "VideoSource");
  ASSERT_EQ(points[0].GetField<double>("cpu"), 0.897657);
  ASSERT_EQ(points[0].GetTime(), "2020-04-08T21:57:04.014Z");

  ASSERT_EQ(points[1].GetMeasurement(), "node");
  ASSERT_EQ(points[1].GetTag("name"), "VideoSource");
  ASSERT_EQ(points[1].GetField<double>("cpu"), 0.982738);
  ASSERT_EQ(points[1].GetTime(), "2020-04-08T21:57:03.768Z");

  ASSERT_EQ(points[2].GetMeasurement(), "node");
  ASSERT_EQ(points[2].GetTag("name"), "InferenceFilter");
  ASSERT_EQ(points[2].GetField<int>("fps"), 10);
  ASSERT_EQ(points[2].GetField<bool>("active"), true);
  ASSERT_EQ(points[2].GetField<std::string>("source"), "something");
  ASSERT_EQ(points[2].GetTime(), "2020-04-08T21:57:04.073Z");
}

class InfluxDbTest : public ::testing::Test {
 public:
  influxdb::Client influx { { detail::utils::localhost, 8086, "test" } };

 protected:
  InfluxDbTest() {
    curl_global_init(CURL_GLOBAL_ALL);
    influx.Query("create database test");
  }

  ~InfluxDbTest() {
    std::string response;
    influx.Query("drop database test");
    curl_global_cleanup();
  }
};

TEST_F(InfluxDbTest, ReadError) {
  std::vector<influxdb::Point> points;
  try {
    influx.Query("not a query", points);
    FAIL() << "expected dcv::detail::HttpError";
  } catch (detail::HttpError const &error) {
  } catch (...) {
    FAIL() << "expected dcv::detail::HttpError";
  }
}

TEST_F(InfluxDbTest, ReadEmptyPoints) {
  std::vector<influxdb::Point> points;
  influx.Query("select * from node", points);
  ASSERT_EQ(points.empty(), true);
}

TEST_F(InfluxDbTest, WritePoint) {
  influx.Write(
    influxdb::Point("node")
      .Tag("name", "InferenceFilter")
      .Field("fps", 10.0)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  std::vector<influxdb::Point> points;
  influx.Query("select * from node", points);
  ASSERT_EQ(points.size(), 1);
  ASSERT_EQ(points[0].GetMeasurement(), "node");
  ASSERT_EQ(points[0].GetField<std::string>("name"), "InferenceFilter");
  ASSERT_EQ(points[0].GetField<double>("fps"), 10.0);
}