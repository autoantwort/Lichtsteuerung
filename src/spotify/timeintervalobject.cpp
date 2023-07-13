#include "timeintervalobject.h"

namespace Spotify::Objects {

TimeIntervalObject::TimeIntervalObject(const QJsonObject &object) : start(object["start"].toDouble()), duration(object["duration"].toDouble()), confidence(object["confidence"].toDouble()) {}

} // namespace Spotify::Objects
