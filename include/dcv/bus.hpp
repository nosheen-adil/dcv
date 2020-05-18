#ifndef DCV_BUS_HPP
#define DCV_BUS_HPP

#include <thread>
#include <zmq.hpp>
#include <flatbuffers/flexbuffers.h>

#include "dcv/influxdb/client.hpp"

namespace dcv {

namespace events {
  const std::string Stop = "stop";
} // namespace events

inline static void Encode(const std::string &str, flexbuffers::Builder &builder) {
  builder.String(str);
  builder.Finish();
}

inline static void Decode(std::string &str, zmq::message_t &message) {
  auto _str = flexbuffers::GetRoot((uint8_t*)message.data(), message.size()).AsString();
  str = _str.str();
}

class Bus : public influxdb::Client {
 public:
  using influxdb::Client::Write;

  Bus(
    const std::string senderAddress,
    const std::string receiverAddress,
    const influxdb::DatabaseConfig databaseConfig
  );

  static std::unique_ptr<Bus> Create(
    const std::string senderAddress = detail::utils::SocketAddress(detail::utils::localhost, 5000),
    const std::string receiverAddress = detail::utils::SocketAddress(detail::utils::localhost, 5001),
    const influxdb::DatabaseConfig databaseConfig = influxdb::DatabaseConfig()
  );
  static std::unique_ptr<Bus> Create(YAML::Node &config);

  void SetLogger(std::shared_ptr<logging::logger> value = nullptr);
  void RegisterCallback(const std::string topic, std::function<void(zmq::message_t&)> callback);
  void RegisterInflux(const std::string measurement, std::function<void(zmq::message_t&)> callback);
  void Start();
  void Send(const std::string topic, zmq::message_t &message);
  template<typename DataType>
  void Send(const std::string topic, const DataType &data);
  void Write(const influxdb::Point &point);
  void Write(const influxdb::Point &&point);
  void Write(const std::vector<influxdb::Point> &points);
  void Stop();
  void CloseSender();
  void Join();
  void Terminate();

 private:
  std::thread worker;
  std::thread terminator;
  std::shared_ptr<logging::logger> logger;
  
  zmq::context_t context { 1 };
  zmq::socket_t sender { context, zmq::socket_type::pub };
  zmq::socket_t receiver { context, zmq::socket_type::sub };
  
  flexbuffers::Builder builder;
  std::unordered_map<std::string, std::function<void(zmq::message_t&)>> callbacks;

  void WorkerRoutine();
  void CloseContext();
};

template<typename DataType>
void Bus::Send(const std::string topic, const DataType &data) {
  Encode(data, builder);
  zmq::message_t message(builder.GetBuffer().data(), builder.GetSize());
  Send(topic, message);
  builder.Clear();
}

} // namespace dcv

#endif // DCV_BUS_HPP