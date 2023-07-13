#include "segmentobject.h"
#include <QJsonArray>

namespace Spotify::Objects {

std::vector<float> getVector(const QJsonValue &value) {
    std::vector<float> vec;
    for (const auto v : value.toArray()) {
        vec.push_back(v.toDouble());
    }
    return vec;
}

SegmentObject::SegmentObject(const QJsonObject &object)
    : start(object["start"].toDouble()), duration(object["duration"].toDouble()), confidence(object["confidence"].toDouble()), loudness_start(object["loudness_start"].toDouble()),
      loudness_max(object["loudness_max"].toDouble()), loudness_max_time(object["loudness_max_time"].toDouble()), loudness_end(object["loudness_end"].toDouble()),
      pitches(getVector(object["pitches"])), timbre(getVector(object["timbre"])) {}

} // namespace Spotify::Objects
