#include <gtest/gtest.h>
#include <dcv/bus.hpp>

using namespace dcv;

class BusTest : public ::testing::Test {
 public:
  YAML::Node config {
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

  std::unique_ptr<Bus> bus;

  zmq::context_t context { 1 };
  zmq::socket_t sender { context, zmq::socket_type::pub };
  zmq::socket_t receiver { context, zmq::socket_type::sub };
  std::thread proxy;
  std::vector<std::string> messages;

  std::string topic = "event";
  std::string message = "hey";
  std::string influxwr = "influx::wr";
  std::string influxack = "influx::ack[node]";
  std::string measurement = "node";

  void Callback(zmq::message_t &message) {
    messages.push_back(message.to_string());
  }

  void TemplateCallback(zmq::message_t &message) {
    std::string str;
    Decode(str, message);
    messages.push_back(str);
  }

  void Create() {
    curl_global_init(CURL_GLOBAL_ALL);
    proxy = std::thread(&BusTest::Proxy, this);
    sender.connect(detail::utils::SocketAddress(detail::utils::localhost, 5000));
    receiver.connect(detail::utils::SocketAddress(detail::utils::localhost, 5001));
    receiver.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    receiver.setsockopt(ZMQ_SUBSCRIBE, influxwr.c_str(), influxwr.size());
    bus = Bus::Create(config);
    bus->Query("create database test");
    bus->SetLogger();
  }

  void Proxy() {
    zmq::socket_t xsender(context, zmq::socket_type::xsub);
    zmq::socket_t xreceiver(context, zmq::socket_type::xpub);

    xsender.bind(detail::utils::SocketAddress(5000));
    xreceiver.bind(detail::utils::SocketAddress(5001));

    try {
      zmq::proxy(xsender, xreceiver, nullptr);
    } catch (zmq::error_t const &error) {
      if (error.num() != ETERM)
        throw error;

      sender.close();
      receiver.close();
    }
  }

 protected:
  ~BusTest() {
    bus->Query("drop database test");
    context.close();
    proxy.join();
    curl_global_cleanup();
  }
};

TEST_F(BusTest, StartTerminate) {
  Create();
  bus->Start();
  bus->Terminate();
}

// TEST_F(BusTest, SendStopErrorContextTerminated) {
//   Create();
//   bus->Start();
//   bus->Stop();

//   try {
//     zmq::message_t message("Message");
//     bus->Send("Event", message);
//     bus->CloseSender();
//     bus->Join();
//     FAIL() << "expected dcv::detail::StopError";
//   } catch (detail::StopError const &error) {
//     logging::info("{}", error.what());
//     ASSERT_EQ(error.what(), std::string("Bus has stopped execution because context was terminated"));
//   } catch (...) {
//     FAIL() << "expected dcv::detail::StopError";
//   }

//   bus->CloseSender();
//   bus->Join();
// }

TEST_F(BusTest, SendStopErrorSocketClosed) {
  Create();
  bus->Start();
  bus->Stop();
  bus->CloseSender();

  try {
    zmq::message_t message("Message");
    bus->Send("Event", message);
    bus->Join();
    FAIL() << "expected dcv::detail::StopError";
  } catch (detail::StopError const &error) {
    ASSERT_EQ(error.what(), std::string("Exception thrown from dcv::Bus::Send():\nInstance has stopped execution - \"Socket was closed\""));
  } catch (...) {
    FAIL() << "expected dcv::detail::StopError but received other error instead";
  }

  bus->Join();
}

TEST_F(BusTest, Send) {
  Create();
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  zmq::message_t i(message);
  bus->Send(topic, i);

  zmq::message_t t;
  zmq::message_t f;
  receiver.recv(t);
  receiver.recv(f);

  ASSERT_EQ(t, zmq::message_t(topic));
  ASSERT_EQ(f, zmq::message_t(message));
  bus->Terminate();
}

TEST_F(BusTest, TemplateSend) {
  Create();
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  bus->Send(topic, message);

  zmq::message_t t;
  zmq::message_t f;
  receiver.recv(t);
  receiver.recv(f);

  ASSERT_EQ(t, zmq::message_t(topic));
  std::string s;
  Decode(s, f);
  ASSERT_EQ(s, message);
  bus->Terminate();
}

TEST_F(BusTest, RegisterCallback) {
  Create();
  bus->RegisterCallback(topic, std::bind(&BusTest::Callback, this, std::placeholders::_1));
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  zmq::message_t t(topic);
  zmq::message_t i(message);
  sender.send(t, zmq::send_flags::sndmore);
  sender.send(i);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_EQ(messages.size(), 1);
  ASSERT_EQ(zmq::message_t(messages[0]), zmq::message_t(message));
  bus->Terminate();
}

TEST_F(BusTest, RegisterTemplateCallback) {
  Create();
  bus->RegisterCallback(topic, std::bind(&BusTest::TemplateCallback, this, std::placeholders::_1));
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  flexbuffers::Builder builder;
  Encode(message, builder);
  zmq::message_t t(topic);
  zmq::message_t i(builder.GetBuffer().data(), builder.GetSize());
  sender.send(t, zmq::send_flags::sndmore);
  sender.send(i);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_EQ(messages.size(), 1);
  ASSERT_EQ(messages[0], std::string("hey"));
  bus->Terminate();
}

TEST_F(BusTest, InfluxWrite) {
  Create();
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  bus->Write(
    influxdb::Point("node")
      .Tag("name", "InferenceFilter")
      .Field("fps", 10.0)
  );

  zmq::message_t t;
  zmq::message_t f;
  receiver.recv(t);
  receiver.recv(f);

  auto p = influxdb::Point("node")
      .Tag("name", "InferenceFilter")
      .Field("fps", 10.0);
  ASSERT_EQ(t, zmq::message_t(influxwr));
  ASSERT_EQ(f, zmq::message_t(p.AsLine()));

  bus->Terminate();
}

TEST_F(BusTest, RegisterInflux) {
  Create();
  bus->RegisterInflux(measurement, std::bind(&BusTest::Callback, this, std::placeholders::_1));
  bus->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  zmq::message_t t(influxack);
  zmq::message_t i(measurement);
  sender.send(t, zmq::send_flags::sndmore);
  sender.send(i);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  ASSERT_EQ(messages.size(), 1);
  ASSERT_EQ(zmq::message_t(messages[0]), zmq::message_t(measurement));
  bus->Terminate();
}