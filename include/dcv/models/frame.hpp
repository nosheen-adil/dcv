#ifndef DCV_DETAIL_MODELS_FRAME_HPP
#define DCV_DETAIL_MODELS_FRAME_HPP

#include <zmq.hpp>
#include <opencv4/opencv2/opencv.hpp>

#include "dcv/models/fbs/frame.h"
#include "dcv/detail/utils.hpp"

namespace dcv {
namespace models {

struct Frame {
  uint64_t id;
  std::string timestamp;
  cv::Mat mat;

  Frame(
    uint64_t id = 0,
    cv::Mat mat = cv::Mat()
  ):
    id(id),
    timestamp(detail::utils::TimestampIso8601()),
    mat(mat) {
  }

  std::string Str() const {
    return "Frame{id=" + std::to_string(id) + ",timestamp=" + timestamp + "}";
  }
};

inline static void Encode(
  const Frame &frame,
  flatbuffers::FlatBufferBuilder &builder
) {
  auto _frame = fbs::CreateFrame(
    builder,
    frame.id,
    builder.CreateString(frame.timestamp),
    fbs::CreateMat(
      builder,
      frame.mat.rows,
      frame.mat.cols,
      frame.mat.type(),
      builder.CreateVector(
        frame.mat.data,
        (frame.mat.cols * frame.mat.rows * frame.mat.channels())
      )
    )
  );

  builder.Finish(_frame);
}

inline static void Decode(
  Frame &frame,
  zmq::message_t &message
) {
  auto _frame = fbs::GetFrame(message.data());

  frame.id = _frame->id();
  frame.timestamp = _frame->timestamp()->str();
  frame.mat = cv::Mat(
    _frame->mat()->rows(),
    _frame->mat()->cols(),
    _frame->mat()->type(),
    (void*)(_frame->mat()->data()->Data())
  );
}

} // naemspace models
} // namespace dcv

#endif // DCV_DETAIL_MODELS_FRAME_HPP