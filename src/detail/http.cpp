#include "dcv/detail/http.hpp"

namespace dcv {
namespace detail {
namespace http {

void InitGetHandle(CURL *getHandle) {
  curl_easy_setopt(getHandle, CURLOPT_SSL_VERIFYPEER, 0); 
  curl_easy_setopt(getHandle, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(getHandle, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(getHandle, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(getHandle, CURLOPT_TCP_KEEPINTVL, 60L);
  curl_easy_setopt(getHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
}

void InitPostHandle(CURL *postHandle) {
  curl_easy_setopt(postHandle, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(postHandle, CURLOPT_CONNECTTIMEOUT, 10);
  curl_easy_setopt(postHandle, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(postHandle, CURLOPT_POST, 1);
  curl_easy_setopt(postHandle, CURLOPT_TCP_KEEPIDLE, 120L);
  curl_easy_setopt(postHandle, CURLOPT_TCP_KEEPINTVL, 60L);
  FILE *devnull = fopen("/dev/null", "w+");
  curl_easy_setopt(postHandle, CURLOPT_WRITEDATA, devnull);
}

void Get(CURL *getHandle, std::string &response) {
  long responseCode;
  curl_easy_setopt(getHandle, CURLOPT_WRITEDATA, &response);
  auto curlCode = curl_easy_perform(getHandle);
  curl_easy_getinfo(getHandle, CURLINFO_RESPONSE_CODE, &responseCode);
  if (curlCode != CURLE_OK)
    throw detail::HttpError("Get", curl_easy_strerror(curlCode));
  if (responseCode < 200 | responseCode > 299)
    throw detail::HttpError("Get", responseCode);
}

void Post(CURL *postHandle, const std::string &post) {
  long responseCode;
  curl_easy_setopt(postHandle, CURLOPT_POSTFIELDS, post.c_str());
  auto curlCode = curl_easy_perform(postHandle);
  curl_easy_getinfo(postHandle, CURLINFO_RESPONSE_CODE, &responseCode);
  if (curlCode != CURLE_OK)
    throw detail::HttpError("Post", curl_easy_strerror(curlCode));
  if (responseCode < 200 | responseCode > 299)
    throw detail::HttpError("Post", responseCode);
}

} // namespace http
} // namespace detail
} // namespace dcv