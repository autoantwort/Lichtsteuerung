#include "trackobject.h"

#include <QJsonArray>
#include <QJsonObject>

namespace Spotify::Objects {

TrackObject_full::TrackObject_full(const QJsonObject &object)
    : album(object[QStringLiteral("album")].toObject())
    , artists(object[QStringLiteral("artists")].toArray())
    , duration_ms(object[QStringLiteral("duration_ms")].toInt())
    , id(object[QStringLiteral("id")].toString())
    , name(object[QStringLiteral("name")].toString())
    , popularity(object[QStringLiteral("popularity")].toInt()) {}

} // namespace Spotify::Objects
