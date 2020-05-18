#ifndef DCV_MANAGER_HPP
#define DCV_MANAGER_HPP

#include <unordered_set>
#include "dcv/bus.hpp"

namespace dcv {

class Manager {
 public:
  Manager(
    const std::string senderAddress,
    const std::string receiverAddress,
    const uint64_t batchSize
  );

  ~Manager();

  static std::unique_ptr<Manager> Create(YAML::Node &config);

  void SetBus(std::unique_ptr<Bus> value = nullptr);
  void Start();
  void Stop();
  void Join();
  void Terminate();

 private:
  std::thread worker;
  std::shared_ptr<logging::logger> logger;

  zmq::context_t context { 1 };
  zmq::socket_t sender { context, zmq::socket_type::xsub };
  zmq::socket_t receiver { context, zmq::socket_type::xpub };

  std::unique_ptr<Bus> bus;
  const uint64_t batchSize;

  std::vector<std::string> batch;
  std::unordered_set<std::string> measurements;

  void InfluxCallback(zmq::message_t &message);
  void StopCallback(zmq::message_t &message);
  void WorkerRoutine();
};

} // namespace dcv

#endif // DCV_MANAGER_HPP