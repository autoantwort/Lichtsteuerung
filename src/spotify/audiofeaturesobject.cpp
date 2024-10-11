#include "audiofeaturesobject.h"

#include <QJsonObject>

namespace Spotify::Objects {

AudioFeaturesObject::AudioFeaturesObject(const QJsonObject &object)
    : acousticness(object[QStringLiteral("acousticness")].toInt())
    , analysis_url(object[QStringLiteral("analysis_url")].toString())
    , danceability(object[QStringLiteral("danceability")].toDouble())
    , duration_ms(object[QStringLiteral("duration_ms")].toInt())
    , energy(object[QStringLiteral("energy")].toDouble())
    , id(object[QStringLiteral("id")].toString())
    , instrumentalness(object[QStringLiteral("instrumentalness")].toDouble())
    , key(object[QStringLiteral("key")].toInt())
    , liveness(object[QStringLiteral("liveness")].toDouble())
    , loudness(object[QStringLiteral("loudness")].toDouble())
    , mode(object[QStringLiteral("mode")].toInt())
    , speechiness(object[QStringLiteral("speechiness")].toDouble())
    , tempo(object[QStringLiteral("tempo")].toDouble())
    , time_signature(object[QStringLiteral("time_signature")].toInt())
    , track_href(object[QStringLiteral("track_href")].toString())
    , valence(object[QStringLiteral("valence")].toDouble()) {}

} // namespace Spotify::Objects
