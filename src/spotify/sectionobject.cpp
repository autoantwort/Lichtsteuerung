#include "sectionobject.h"

namespace Spotify::Objects {

SectionObject::SectionObject(const QJsonObject &object)
    : start(object["start"].toDouble()), duration(object["duration"].toDouble()), confidence(object["confidence"].toDouble()), loudness(object["loudness"].toDouble()),
      tempo(object["tempo"].toDouble()), tempo_confidence(object["tempo_confidence"].toDouble()), key(object["key"].toInt()), key_confidence(object["key_confidence"].toDouble()),
      mode(object["mode"].toInt()), mode_confidence(object["mode_confidence"].toDouble()), time_signature(object["time_signature"].toInt()),
      time_signature_confidence(object["time_signature_confidence"].toDouble()) {}

} // namespace Spotify::Objects
