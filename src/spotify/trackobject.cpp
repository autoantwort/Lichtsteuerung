#include "trackobject.h"

namespace Spotify::Objects {

TrackObject_full::TrackObject_full(const QJsonObject &object)
    : album(object["album"].toObject())
    , artists(object["artists"].toArray())
    , duration_ms(object["duration_ms"].toInt())
    , id(object["id"].toString())
    , name(object["name"].toString())
    , popularity(object["popularity"].toInt()) {}

} // namespace Spotify::Objects
