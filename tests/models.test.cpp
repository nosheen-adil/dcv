#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dcv/models.hpp>

using namespace dcv::models;
using namespace dcv::detail;

class FrameTest : public ::testing::Test {
 public:
  uint64_t id { 321212 };
  float data[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 10 };
  cv::Mat mat = cv::Mat(1, 10, CV_32F, data);
};

TEST_F(FrameTest, Constructor) {
  Frame frame(id, mat);
  const auto then = utils::TimestampUnix(frame.timestamp);
  const auto now = utils::TimestampUnix();

  ASSERT_EQ(frame.id, id);
  ASSERT_GE(now, then);
  ASSERT_EQ(utils::MatsEqual(frame.mat, mat), true);
}

TEST_F(FrameTest, EncodeDecode) {
  Frame i(id, mat);
  flatbuffers::FlatBufferBuilder builder;
  Encode(i, builder);
  
  zmq::message_t message(builder.GetBufferPointer(), builder.GetSize());
  Frame f;
  Decode(f, message);

  ASSERT_EQ(f.id, i.id);
  ASSERT_EQ(f.timestamp, i.timestamp);
  ASSERT_EQ(utils::MatsEqual(f.mat, i.mat), true);
}

class DetectionsTest : public ::testing::Test {
 public:
  uint64_t id { 95784 };
  std::vector<Detection> data {
    { 0.78954, "person", {1, 2, 3, 4} },
    { 0.5603, "bunny", {34, 56, 78, 91} },
    { 0.8912, "cat", {12, 37, 42, 43} }
  };
};

TEST_F(DetectionsTest, DefaultConstructor) {
  Detections detections(id, data);
  const auto then = utils::TimestampUnix(detections.timestamp);
  const auto now = utils::TimestampUnix();

  ASSERT_EQ(detections.id, id);
  ASSERT_GE(now, then);

  const auto size = data.size();
  ASSERT_EQ(detections.data.size(), size);
  for (auto i = 0; i < size; i++) {
    const auto actualDetection = detections.data.at(i);
    const auto expectedDetection = data.at(i);
    ASSERT_EQ(actualDetection.id, expectedDetection.id);
    ASSERT_EQ(actualDetection.conf, expectedDetection.conf);
    ASSERT_EQ(actualDetection.label, expectedDetection.label);
    ASSERT_EQ(actualDetection.bbox, expectedDetection.bbox);
  }
}

TEST_F(DetectionsTest, EncodeDecode) {
  Detections i(id, data);
  flatbuffers::FlatBufferBuilder builder;
  Encode(i, builder);
  
  zmq::message_t message(builder.GetBufferPointer(), builder.GetSize());
  Detections f;
  Decode(f, message);

  ASSERT_EQ(f.id, i.id);
  ASSERT_EQ(f.timestamp, i.timestamp);
    
  const auto size = i.data.size();
  ASSERT_EQ(f.data.size(), size);
  for (auto j = 0; j < size; j++) {
    const auto actualDetection = f.data.at(j);
    const auto expectedDetection = i.data.at(j);
    ASSERT_EQ(actualDetection.id, expectedDetection.id);
    ASSERT_EQ(actualDetection.conf, expectedDetection.conf);
    ASSERT_EQ(actualDetection.label, expectedDetection.label);
    ASSERT_EQ(actualDetection.bbox, expectedDetection.bbox);
  }
}