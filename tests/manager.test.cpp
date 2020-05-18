#include <gtest/gtest.h>
#include <dcv/manager.hpp>

using namespace dcv;

class ManagerTest : public ::testing::Test {
 public:
  YAML::Node busConfig {
    YAML::Load(R"(
      sender:
        ip: "127.0.0.1"
        port: 5000
      receiver:
        ip: "127.0.0.1"
        port: 5001
      influxdb:
        database: "test"
    )")
  };

  YAML::Node managerConfig {
    YAML::Load(R"(
      batchSize: 1000
      bus:
        sender:
          ip: "127.0.0.1"
          port: 5000
        receiver:
          ip: "127.0.0.1"
          port: 5001
        influxdb:
          database: "test"
      proxy:
        sender:
          port: 5000
        receiver:
          port: 5001
    )")
  };

  std::unique_ptr<Bus> bus;
  std::unique_ptr<Manager> manager;
  std::vector<std::string> messages;

  void Create() {
    bus = Bus::Create(busConfig);
    manager = Manager::Create(managerConfig);
    bus->Query("create database test");
  }

  void Callback(zmq::message_t &message) {
    messages.push_back(message.to_string());
  }

  protected:
  ~ManagerTest() {
    bus->Query("drop database test");
  }
};

TEST_F(ManagerTest, StartTerminate) {
  Create();
  manager->Start();
  manager->Terminate();
}

TEST_F(ManagerTest, WriteBatch) {
  const int batchSize = 3;
  std::vector<influxdb::Point> points;
  managerConfig["batchSize"] = batchSize;
  
  Create();
  manager->Start();
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  bus->Write(
    influxdb::Point("node")
      .Tag("name", "InferenceFilter")
      .Field("fps", 9.64723)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  bus->Query("select * from node", points);
  ASSERT_EQ(points.size(), 0);

  bus->Write(
    influxdb::Point("node")
      .Tag("name", "VideoSource")
      .Field("fps", 10.2903)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  bus->Query("select * from node", points);
  ASSERT_EQ(points.size(), 0);

  bus->Write(
    influxdb::Point("node")
      .Tag("name", "VideoSink")
      .Field("fps", 7.82913)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  
  bus->Write(
    influxdb::Point("node")
      .Tag("name", "VideoSink")
      .Field("fps", 7.82913)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  bus->Query("select * from node", points);
  ASSERT_EQ(points.size(), batchSize);

  ASSERT_EQ(points[0].GetMeasurement(), "node");
  ASSERT_EQ(points[0].GetField<std::string>("name"), "InferenceFilter");
  ASSERT_EQ(points[0].GetField<double>("fps"), 9.64723);

  ASSERT_EQ(points[1].GetMeasurement(), "node");
  ASSERT_EQ(points[1].GetField<std::string>("name"), "VideoSource");
  ASSERT_EQ(points[1].GetField<double>("fps"), 10.2903);

  ASSERT_EQ(points[2].GetMeasurement(), "node");
  ASSERT_EQ(points[2].GetField<std::string>("name"), "VideoSink");
  ASSERT_EQ(points[2].GetField<double>("fps"), 7.82913);

  bus->Terminate();
  manager->Terminate();
}

TEST_F(ManagerTest, MeasurementCallback) {
  const int batchSize = 3;
  std::vector<influxdb::Point> points;
  managerConfig["batchSize"] = batchSize;
  
  Create();
  manager->Start();
  bus->RegisterInflux("node", std::bind(&ManagerTest::Callback, this, std::placeholders::_1));
  bus->RegisterInflux("analytics", std::bind(&ManagerTest::Callback, this, std::placeholders::_1));
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  bus->Write(
    influxdb::Point("node")
      .Tag("name", "InferenceFilter")
      .Field("fps", 9.64723)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  bus->Query("select * from node", points);
  ASSERT_EQ(points.size(), 0);

  bus->Write(
    influxdb::Point("analytics")
      .Field("people_counter", 10)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  bus->Query("select * from node", points);
  ASSERT_EQ(points.size(), 0);

  bus->Write(
    influxdb::Point("node")
      .Tag("name", "VideoSink")
      .Field("fps", 7.82913)
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  ASSERT_EQ(messages.size(), 2);
  ASSERT_EQ(messages[0], std::string("analytics"));
  ASSERT_EQ(messages[1], std::string("node"));
  bus->Terminate();
  manager->Terminate();
}