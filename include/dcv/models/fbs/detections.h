// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_DETECTIONS_DCV_MODELS_FBS_H_
#define FLATBUFFERS_GENERATED_DETECTIONS_DCV_MODELS_FBS_H_

#include "flatbuffers/flatbuffers.h"

namespace dcv {
namespace models {
namespace fbs {

struct Box;
struct BoxBuilder;

struct Detection;
struct DetectionBuilder;

struct Detections;
struct DetectionsBuilder;

struct Box FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef BoxBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_WIDTH = 8,
    VT_HEIGHT = 10
  };
  int32_t x() const {
    return GetField<int32_t>(VT_X, 0);
  }
  int32_t y() const {
    return GetField<int32_t>(VT_Y, 0);
  }
  int32_t width() const {
    return GetField<int32_t>(VT_WIDTH, 0);
  }
  int32_t height() const {
    return GetField<int32_t>(VT_HEIGHT, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_X) &&
           VerifyField<int32_t>(verifier, VT_Y) &&
           VerifyField<int32_t>(verifier, VT_WIDTH) &&
           VerifyField<int32_t>(verifier, VT_HEIGHT) &&
           verifier.EndTable();
  }
};

struct BoxBuilder {
  typedef Box Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(int32_t x) {
    fbb_.AddElement<int32_t>(Box::VT_X, x, 0);
  }
  void add_y(int32_t y) {
    fbb_.AddElement<int32_t>(Box::VT_Y, y, 0);
  }
  void add_width(int32_t width) {
    fbb_.AddElement<int32_t>(Box::VT_WIDTH, width, 0);
  }
  void add_height(int32_t height) {
    fbb_.AddElement<int32_t>(Box::VT_HEIGHT, height, 0);
  }
  explicit BoxBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Box> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Box>(end);
    return o;
  }
};

inline flatbuffers::Offset<Box> CreateBox(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t x = 0,
    int32_t y = 0,
    int32_t width = 0,
    int32_t height = 0) {
  BoxBuilder builder_(_fbb);
  builder_.add_height(height);
  builder_.add_width(width);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Detection FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef DetectionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_CONF = 6,
    VT_LABEL = 8,
    VT_BBOX = 10
  };
  uint64_t id() const {
    return GetField<uint64_t>(VT_ID, 0);
  }
  float conf() const {
    return GetField<float>(VT_CONF, 0.0f);
  }
  const flatbuffers::String *label() const {
    return GetPointer<const flatbuffers::String *>(VT_LABEL);
  }
  const dcv::models::fbs::Box *bbox() const {
    return GetPointer<const dcv::models::fbs::Box *>(VT_BBOX);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_ID) &&
           VerifyField<float>(verifier, VT_CONF) &&
           VerifyOffset(verifier, VT_LABEL) &&
           verifier.VerifyString(label()) &&
           VerifyOffset(verifier, VT_BBOX) &&
           verifier.VerifyTable(bbox()) &&
           verifier.EndTable();
  }
};

struct DetectionBuilder {
  typedef Detection Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint64_t id) {
    fbb_.AddElement<uint64_t>(Detection::VT_ID, id, 0);
  }
  void add_conf(float conf) {
    fbb_.AddElement<float>(Detection::VT_CONF, conf, 0.0f);
  }
  void add_label(flatbuffers::Offset<flatbuffers::String> label) {
    fbb_.AddOffset(Detection::VT_LABEL, label);
  }
  void add_bbox(flatbuffers::Offset<dcv::models::fbs::Box> bbox) {
    fbb_.AddOffset(Detection::VT_BBOX, bbox);
  }
  explicit DetectionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Detection> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Detection>(end);
    return o;
  }
};

inline flatbuffers::Offset<Detection> CreateDetection(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    float conf = 0.0f,
    flatbuffers::Offset<flatbuffers::String> label = 0,
    flatbuffers::Offset<dcv::models::fbs::Box> bbox = 0) {
  DetectionBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_bbox(bbox);
  builder_.add_label(label);
  builder_.add_conf(conf);
  return builder_.Finish();
}

inline flatbuffers::Offset<Detection> CreateDetectionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    float conf = 0.0f,
    const char *label = nullptr,
    flatbuffers::Offset<dcv::models::fbs::Box> bbox = 0) {
  auto label__ = label ? _fbb.CreateString(label) : 0;
  return dcv::models::fbs::CreateDetection(
      _fbb,
      id,
      conf,
      label__,
      bbox);
}

struct Detections FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef DetectionsBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_TIMESTAMP = 6,
    VT_DATA = 8
  };
  uint64_t id() const {
    return GetField<uint64_t>(VT_ID, 0);
  }
  const flatbuffers::String *timestamp() const {
    return GetPointer<const flatbuffers::String *>(VT_TIMESTAMP);
  }
  const flatbuffers::Vector<flatbuffers::Offset<dcv::models::fbs::Detection>> *data() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<dcv::models::fbs::Detection>> *>(VT_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_TIMESTAMP) &&
           verifier.VerifyString(timestamp()) &&
           VerifyOffset(verifier, VT_DATA) &&
           verifier.VerifyVector(data()) &&
           verifier.VerifyVectorOfTables(data()) &&
           verifier.EndTable();
  }
};

struct DetectionsBuilder {
  typedef Detections Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint64_t id) {
    fbb_.AddElement<uint64_t>(Detections::VT_ID, id, 0);
  }
  void add_timestamp(flatbuffers::Offset<flatbuffers::String> timestamp) {
    fbb_.AddOffset(Detections::VT_TIMESTAMP, timestamp);
  }
  void add_data(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<dcv::models::fbs::Detection>>> data) {
    fbb_.AddOffset(Detections::VT_DATA, data);
  }
  explicit DetectionsBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Detections> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Detections>(end);
    return o;
  }
};

inline flatbuffers::Offset<Detections> CreateDetections(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    flatbuffers::Offset<flatbuffers::String> timestamp = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<dcv::models::fbs::Detection>>> data = 0) {
  DetectionsBuilder builder_(_fbb);
  builder_.add_id(id);
  builder_.add_data(data);
  builder_.add_timestamp(timestamp);
  return builder_.Finish();
}

inline flatbuffers::Offset<Detections> CreateDetectionsDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t id = 0,
    const char *timestamp = nullptr,
    const std::vector<flatbuffers::Offset<dcv::models::fbs::Detection>> *data = nullptr) {
  auto timestamp__ = timestamp ? _fbb.CreateString(timestamp) : 0;
  auto data__ = data ? _fbb.CreateVector<flatbuffers::Offset<dcv::models::fbs::Detection>>(*data) : 0;
  return dcv::models::fbs::CreateDetections(
      _fbb,
      id,
      timestamp__,
      data__);
}

inline const dcv::models::fbs::Detections *GetDetections(const void *buf) {
  return flatbuffers::GetRoot<dcv::models::fbs::Detections>(buf);
}

inline const dcv::models::fbs::Detections *GetSizePrefixedDetections(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<dcv::models::fbs::Detections>(buf);
}

inline bool VerifyDetectionsBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<dcv::models::fbs::Detections>(nullptr);
}

inline bool VerifySizePrefixedDetectionsBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<dcv::models::fbs::Detections>(nullptr);
}

inline void FinishDetectionsBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<dcv::models::fbs::Detections> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedDetectionsBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<dcv::models::fbs::Detections> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace fbs
}  // namespace models
}  // namespace dcv

#endif  // FLATBUFFERS_GENERATED_DETECTIONS_DCV_MODELS_FBS_H_
