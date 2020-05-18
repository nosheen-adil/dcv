#ifndef DCV_DETAIL_TOOLS_HTTP_HPP
#define DCV_DETAIL_TOOLS_HTTP_HPP

#include <string>
#include <thread>
#include <curl/curl.h>
#include <rapidjson/document.h>

#include <dcv/detail/logging.hpp>
#include "dcv/detail/error.hpp"

namespace dcv {
namespace detail {
namespace http {

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

void InitGetHandle(CURL *getHandle);
void InitPostHandle(CURL *postHandle);
void Get(CURL *getHandle, std::string &response);
void Post(CURL *postHandle, const std::string &post);

} // namespace http
} // namespace detail
} // namespace dcv

#endif // DCV_DETAIL_TOOLS_HTTP_HPP