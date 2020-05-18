#include "dcv/manager.hpp"

namespace dcv {

Manager::Manager(
  const std::string senderAddress,
  const std::string receiverAddress,
  const uint64_t batchSize
): 
  batchSize(batchSize) {
  detail::SetLogger(logger, "Manager");

  int linger = 0;
  sender.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
  receiver.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
  
  sender.bind(senderAddress);
  receiver.bind(receiverAddress);
}

Manager::~Manager() {
  logging::drop("Manager");
}

std::unique_ptr<Manager> Manager::Create(YAML::Node &config) {
  uint64_t batchSize = 100;
  if (config["batchSize"])
    batchSize = config["batchSize"].as<uint64_t>();

  auto proxy = config["proxy"];
  auto manager = std::unique_ptr<Manager>(new Manager(
    detail::utils::SocketAddress(proxy["sender"]),
    detail::utils::SocketAddress(proxy["receiver"]),
    batchSize
  ));
  
  auto busConfig = config["bus"];
  manager->SetBus(Bus::Create(busConfig));
  return manager;
}

void Manager::SetBus(std::unique_ptr<Bus> value) {
  bus = value ? std::move(value) : Bus::Create();
}

void Manager::Start() {
  logger->info("Manager::Start()");
  if (!bus) SetBus();
  bus->SetLogger(logger);
  bus->RegisterCallback("influx::wr", std::bind(&Manager::InfluxCallback, this, std::placeholders::_1));
  bus->RegisterCallback(events::Stop, std::bind(&Manager::StopCallback, this, std::placeholders::_1));
  
  auto dc = bus->GetDatabase();
  bus->Query("create database " + dc.database);

  bus->Start();
  worker = std::thread(&Manager::WorkerRoutine, this);
}

void Manager::Stop() {
  logger->info("Manager::Stop()");
  bus->Stop();
  bus->CloseSender();
  context.shutdown();
  context.close();
}

void Manager::Join() {
  bus->Join();
  worker.join();
  logger->info("Manager::Join() - Finished!");
}

void Manager::Terminate() {
  Stop();
  Join();
}

void Manager::InfluxCallback(zmq::message_t &message) {
  try {
    auto line = message.to_string();
    batch.push_back(line);
    auto comma = line.find(",");
    auto space = line.find(" ");
    size_t pos;
    if (comma == std::string::npos) {
      pos = space;
    } else if (comma < space) {
      pos = comma;
    } else {
      pos = space;
    }
  
    auto measurement = line.substr(0, pos);
    measurements.insert(measurement);
    logger->debug("Manager::InfluxCallback() - line = \"{}\" batch.size()={}", line, batch.size());
    
    if (batch.size() == batchSize) {
      logger->info("Manager::InfluxCallback() - Writing batch");
      bus->Write(batch);
      for (auto measurement : measurements) {
        logger->info("Manager::InfluxCallback() - Sending measurement = {}", measurement);
        zmq::message_t message(measurement);
        bus->Send("influx::ack[" + measurement + "]", message);
      }
      batch.clear();
      measurements.clear();
    }
  } catch (detail::StopError const &error) {
    logger->warn("Manager::InfluxCallback() - Caught stop exception");
  }
}

void Manager::StopCallback(zmq::message_t &message) {
  Stop();
}

void Manager::WorkerRoutine() {
  logger->info("Manager::WorkerRoutine() - Starting proxy");
  try {
    zmq::proxy(sender, receiver, nullptr);
  } catch (zmq::error_t const &error) {
    if (error.num() != ETERM)
      throw error;

    sender.close();
    receiver.close();
  }
}

} // namespace dcv