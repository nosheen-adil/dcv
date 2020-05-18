#include "dcv/bus.hpp"

namespace dcv {

Bus::Bus(
  const std::string senderAddress,
  const std::string receiverAddress,
  const influxdb::DatabaseConfig databaseConfig
): influxdb::Client(databaseConfig) {
  int linger = 0;
  sender.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
  receiver.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

  sender.connect(senderAddress);
  receiver.connect(receiverAddress);
}

std::unique_ptr<Bus> Bus::Create(
  const std::string senderAddress,
  const std::string receiverAddress,
  const influxdb::DatabaseConfig databaseConfig
) {
  return std::unique_ptr<Bus>(new Bus(
    senderAddress, receiverAddress, databaseConfig
  ));
}

std::unique_ptr<Bus> Bus::Create(YAML::Node &config) {
  auto sender = config["sender"];
  auto receiver = config["receiver"];
  return std::unique_ptr<Bus>(new Bus(
    detail::utils::SocketAddress(config["sender"]),
    detail::utils::SocketAddress(config["receiver"]),
    influxdb::DatabaseConfig::FromYaml(config["influxdb"])
  ));
}

void Bus::SetLogger(std::shared_ptr<logging::logger> value) {
  if (!value) {
    logging::drop("Bus");
    logger = logging::basic_logger_mt("Bus", "/var/log/dcv/Bus.log");
  } else {
    logger = std::move(value); 
  }
}

void Bus::RegisterCallback(const std::string topic, std::function<void(zmq::message_t&)> callback) {
  if (!logger) SetLogger();
  callbacks[topic] = callback;
  receiver.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
  logger->info("Bus::RegisterCallback() - topic = {}", topic);
}

void Bus::RegisterInflux(const std::string measurement, std::function<void(zmq::message_t&)> callback) {
  if (!logger) SetLogger();
  const auto topic = "influx::ack[" + measurement + "]";
  RegisterCallback(topic, callback);
}

void Bus::Start() {
  if (!logger) SetLogger();
  logger->info("Bus::Start()");
  worker = std::thread(&Bus::WorkerRoutine, this);
}

void Bus::Send(const std::string topic, zmq::message_t &message) {
  try {
    sender.send(zmq::message_t(topic), zmq::send_flags::sndmore);
    sender.send(message);
    logger->trace("Bus::Send() - topic = {}", topic);
  } catch (zmq::error_t const &error) {
    if (error.num() == ETERM) {
      logger->info("Bus::Send() - Context terminated, closing sender and shutting down");
      sender.close();
      throw detail::StopError("Bus", "Send", "Context was terminated");
    }

    if (error.num() == ENOTSOCK) {
      logger->info("Bus::Send() - Sender closed, shutting down");
      throw detail::StopError("Bus", "Send", "Socket was closed");
    }

    throw error;
  }    
}

void Bus::Write(const influxdb::Point &point) {
  std::string topic = "influx::wr";
  zmq::message_t message(point.AsLine());
  Send(topic, message);
}

void Bus::Write(const influxdb::Point &&point) {
  std::string topic = "influx::wr";
  zmq::message_t message(point.AsLine());
  Send(topic, message);
}

void Bus::Write(const std::vector<influxdb::Point> &points) {
  for (auto &point : points) {
    Write(point);
  }
}

void Bus::Stop() {
  logger->info("Bus::Stop()");
  terminator = std::thread(&Bus::CloseContext, this);   
}

void Bus::CloseSender() {
  logger->info("Bus::CloseSender()");
  try {
    sender.close();
  } catch (zmq::error_t const &error) {
    if (error.num() == ENOTSOCK)
      return;

    throw error;
  }
}

void Bus::Join() {
  worker.join();
  terminator.join();
  logger->info("Bus::Join() - Finished!");
}

void Bus::Terminate() {
  Stop();
  CloseSender();
  Join();
}

void Bus::WorkerRoutine() {
  try {
    while (true) {
      zmq::message_t topic;
      zmq::message_t message;

      receiver.recv(topic);
      receiver.recv(message);
      
      auto topicString = topic.to_string();
      logger->trace("Bus::WorkerRoutine() - topic = {}", topicString);

      if (callbacks.count(topicString)) {
        callbacks.at(topicString)(message);
      } else {
        logger->trace("Bus::WorkerRoutine() - No callback match for topic");
      }
    }
  } catch (zmq::error_t const &error) {
    if (error.num() == ETERM) {
      logger->info("Bus::WokerRoutine() - Context terminated, closing receiver and shutting down");
      receiver.close();
      return;
    }

    throw error;
  }
}

void Bus::CloseContext() {
  logger->info("Bus::CloseContext() - Closing context");
  context.shutdown();
  context.close();
}

} // namespace dcv