#ifndef DCV_MODELS_DETECTIONS_HPP
#define DCV_MODELS_DETECTIONS_HPP

#include <zmq.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include "dcv/models/fbs/detections.h"
#include "dcv/detail/utils.hpp"

namespace dcv {
namespace models {

struct Detection {
 public:
  uint64_t id;
  float conf;
  std::string label;
  cv::Rect bbox;

  Detection(
    uint64_t id = 0,
    float conf = 0.0f,
    std::string label = "",
    cv::Rect bbox = cv::Rect()
  ):
    id(id),
    conf(conf),
    label(label),
    bbox(bbox) {
  }

  Detection(
    float conf,
    std::string label,
    cv::Rect bbox
  ):
    id(0),
    conf(conf),
    label(label),
    bbox(bbox) {
  }

  void CheckBboxDims(const int width, const int height) {
    if (bbox.x < 0)
      bbox.x = 0;
    if (bbox.y < 0)
      bbox.y = 0;
    if ((bbox.x + bbox.width) > width)
      bbox.width = width - bbox.x;
    if ((bbox.y + bbox.height) > height)
      bbox.height = height - bbox.y;
  }

  std::string Str() const {
    std::stringstream ss;
    ss << bbox;
    return "Detection{id=" + std::to_string(id) + ",conf=" + std::to_string(conf) + ",label=" + label + ",bbox=" + ss.str() + "}";
  }
};

struct Detections {
  uint64_t id;
  std::string timestamp;
  std::vector<Detection> data;

  Detections(
    uint64_t id = 0,
    std::vector<Detection> data = {}
  ):
    id(id),
    timestamp(detail::utils::TimestampIso8601()),
    data(data) {
  }

  std::string Str() const {
    std::string d = "Detections{id=" + std::to_string(id) + ",timestamp=" + timestamp + ",data=[\n";
    bool first = false;
    for (auto _d : data) {
      d += _d.Str();
      d += "\n";
    }
    d +="]}";
    return d;
  }
};

inline static void Encode(
  const Detections &detections,
  flatbuffers::FlatBufferBuilder &builder
) {
  std::vector<flatbuffers::Offset<fbs::Detection>> _data;
  for (auto detection : detections.data) {
    _data.push_back(
      fbs::CreateDetection(
        builder,
        detection.id,
        detection.conf,
        builder.CreateString(detection.label),
        fbs::CreateBox(
          builder,
          detection.bbox.x,
          detection.bbox.y,
          detection.bbox.width,
          detection.bbox.height
        )
      )
    );
  }

  auto _detections = fbs::CreateDetections(
    builder,
    detections.id,
    builder.CreateString(detections.timestamp),
    builder.CreateVector(_data)
  );

  builder.Finish(_detections);
}

inline static void Decode(
  Detections &detections,
  zmq::message_t &message
) {
  auto _detections = fbs::GetDetections(message.data());
  detections.id = _detections->id();
  detections.timestamp = _detections->timestamp()->str();

  auto _data = _detections->data();
  for (int i = 0; i < _data->Length(); i++) {
    auto _detection = _data->Get(i);
    Detection detection;
    detection.id = _detection->id();
    detection.conf = _detection->conf();
    detection.label = _detection->label()->str();
    detection.bbox = cv::Rect(
      _detection->bbox()->x(),
      _detection->bbox()->y(),
      _detection->bbox()->width(),
      _detection->bbox()->height()
    );
    detections.data.push_back(detection);
  }
}

} // namespace models
} // namespace dcv

#endif // DCV_MODELS_DETECTIONS_HPP