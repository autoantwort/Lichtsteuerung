#include "segmentobject.h"
#include <QJsonArray>
#include <QJsonObject>

namespace Spotify::Objects {

std::vector<float> getVector(const QJsonValue &value) {
    std::vector<float> vec;
    for (const auto v : value.toArray()) {
        vec.push_back(v.toDouble());
    }
    return vec;
}

SegmentObject::SegmentObject(const QJsonObject &object)
    : start(object[QStringLiteral("start")].toDouble())
    , duration(object[QStringLiteral("duration")].toDouble())
    , confidence(object[QStringLiteral("confidence")].toDouble())
    , loudness_start(object[QStringLiteral("loudness_start")].toDouble())
    , loudness_max(object[QStringLiteral("loudness_max")].toDouble())
    , loudness_max_time(object[QStringLiteral("loudness_max_time")].toDouble())
    , loudness_end(object[QStringLiteral("loudness_end")].toDouble())
    , pitches(getVector(object[QStringLiteral("pitches")]))
    , timbre(getVector(object[QStringLiteral("timbre")])) {}

} // namespace Spotify::Objects
