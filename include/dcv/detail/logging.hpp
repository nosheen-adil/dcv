#ifndef DCV_DETAIL_LOGGING_HPP
#define DCV_DETAIL_LOGGING_HPP

#include <cstdlib>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace logging = spdlog;

namespace dcv {
namespace detail {

static void InitLogging() {
  char const* level = std::getenv("SP_LOGGING_LEVEL");
  if (level == NULL) {
    logging::set_level(logging::level::info);
  } else {
    if (!std::strcmp(level, "TRACE")) {
      logging::set_level(logging::level::trace);
    } else if (!std::strcmp(level, "DEBUG")) {
      logging::set_level(logging::level::debug);
    } else if (!std::strcmp(level, "WARN")) {
      logging::set_level(logging::level::warn);
    } else if (!std::strcmp(level, "ERROR")) {
      logging::set_level(logging::level::err);
    } else if (!std::strcmp(level, "CRITICAL")) {
      logging::set_level(logging::level::critical);
    } else if (!std::strcmp(level, "OFF")) {
      logging::set_level(logging::level::off);
    } else {
      logging::set_level(logging::level::info);
    }
  }
  logging::flush_on(logging::level::info);
}

static void SetLogger(std::shared_ptr<logging::logger> &logger, const std::string name) {
  try {
    logger = logging::basic_logger_mt(name, "/var/log/streamproc/" + name + ".log");
  } catch (...) {
    auto offset = 0;
    while (true) {
      try {
        logger = logging::basic_logger_mt(name + std::to_string(offset), "/var/log/streamproc/" + name + std::to_string(offset) + ".log");
        break;
      } catch (...) {
        offset++;
      }
    } 
  }
}


} // namespace detail
} // namespace dcv

#endif // DCV_DETAIL_LOGGING_HPP