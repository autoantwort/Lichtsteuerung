#include "timeintervalobject.h"
#include <QJsonObject>

namespace Spotify::Objects {

TimeIntervalObject::TimeIntervalObject(const QJsonObject &object)
    : start(object[QStringLiteral("start")].toDouble())
    , duration(object[QStringLiteral("duration")].toDouble())
    , confidence(object[QStringLiteral("confidence")].toDouble()) {}

} // namespace Spotify::Objects
