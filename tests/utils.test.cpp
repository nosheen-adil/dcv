#include <gtest/gtest.h>
#include <dcv/detail/utils.hpp>

using namespace dcv::detail;

TEST(UtilsTest, SocketAddress) {
  ASSERT_EQ(utils::SocketAddress("endpoint", "ipc"), "ipc://endpoint");
  ASSERT_EQ(utils::SocketAddress("192.168.100.219", 5000), "tcp://192.168.100.219:5000");
  ASSERT_EQ(utils::SocketAddress(6000), "tcp://*:6000");
}

TEST(UtilsTest, MatsEqual) {
  float data[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 10 };
  cv::Mat m1 = cv::Mat(1, 10, CV_32F, data);
  cv::Mat m2 = cv::Mat(1, 10, CV_32F, data);
  ASSERT_EQ(utils::MatsEqual(m1, m2, 0), true);
}

TEST(UtilsTest, TimestampIso8601Seconds) {
  std::string i = "2020-04-08T21:57:03Z";
  auto t = utils::Timepoint(i);
  std::string f = utils::TimestampIso8601(t, TimestampPrecision::s);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampIso8601Milliseconds) {
  std::string i = "1992-07-20T12:18:09.123Z";
  auto t = utils::Timepoint(i);
  std::string f = utils::TimestampIso8601(t, TimestampPrecision::ms);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampIso8601Microseconds) {
  std::string i = "2000-10-15T03:42:21.123666Z";
  auto t = utils::Timepoint(i);
  std::string f = utils::TimestampIso8601(t, TimestampPrecision::u);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampIso8601Nanoseconds) {
  std::string i = "2000-10-15T03:42:21.123666798Z";
  auto t = utils::Timepoint(i);
  std::string f = utils::TimestampIso8601(t, TimestampPrecision::ns);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampUnixSeconds) {
  uint64_t i = 1586465668;
  auto t = utils::Timepoint(i, TimestampPrecision::s);
  uint64_t f = utils::TimestampUnix(t, TimestampPrecision::s);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampUnixMilliseconds) {
  uint64_t i = 1586465919536;
  auto t = utils::Timepoint(i, TimestampPrecision::ms);
  uint64_t f = utils::TimestampUnix(t, TimestampPrecision::ms);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampUnixMicroseconds) {
  uint64_t i = 1586465919536602;
  auto t = utils::Timepoint(i, TimestampPrecision::u);
  uint64_t f = utils::TimestampUnix(t, TimestampPrecision::u);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampUnixNanoseconds) {
  uint64_t i = 1586465919536694;
  auto t = utils::Timepoint(i, TimestampPrecision::ns);
  uint64_t f = utils::TimestampUnix(t, TimestampPrecision::ns);
  ASSERT_EQ(f, i);
}

TEST(UtilsTest, TimestampIso8601UnixSeconds) {
  std::string i = "2020-04-08T21:57:03Z";
  uint64_t u = 1586383023;
  
  ASSERT_EQ(utils::TimestampIso8601(u, TimestampPrecision::s), i);
  ASSERT_EQ(utils::TimestampUnix(i, TimestampPrecision::s), u);
}

TEST(UtilsTest, TimestampIso8601UnixMilliseconds) {
  std::string i = "1992-07-20T12:18:09.123Z";
  uint64_t u = 711634689123;
  
  ASSERT_EQ(utils::TimestampIso8601(u, TimestampPrecision::ms), i);
  ASSERT_EQ(utils::TimestampUnix(i, TimestampPrecision::ms), u);
}

TEST(UtilsTest, TimestampIso8601UnixMicroseconds) {
  std::string i = "1992-07-20T12:18:09.123456Z";
  uint64_t u = 711634689123456;
  
  ASSERT_EQ(utils::TimestampIso8601(u, TimestampPrecision::u), i);
  ASSERT_EQ(utils::TimestampUnix(i, TimestampPrecision::u), u);
}

TEST(UtilsTest, TimestampIso8601UnixNanoseconds) {
  std::string i = "1992-07-20T12:18:09.123456789Z";
  uint64_t u = 711634689123456789;
  
  ASSERT_EQ(utils::TimestampIso8601(u, TimestampPrecision::ns), i);
  ASSERT_EQ(utils::TimestampUnix(i, TimestampPrecision::ns), u);
}