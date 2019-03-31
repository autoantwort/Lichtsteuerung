#include "audiofeaturesobject.h"

namespace Spotify::Objects{

AudioFeaturesObject::AudioFeaturesObject(const QJsonObject &object) :
    acousticness(object["acousticness"].toInt()),
    analysis_url(object["analysis_url"].toString()),
    danceability(object["danceability"].toDouble()),
    duration_ms(object["duration_ms"].toInt()),
    energy(object["energy"].toDouble()),
    id(object["id"].toString()),
    instrumentalness(object["instrumentalness"].toDouble()),
    key(object["key"].toInt()),
    liveness(object["liveness"].toDouble()),
    loudness(object["loudness"].toDouble()),
    mode(object["mode"].toInt()),
    speechiness(object["speechiness"].toDouble()),
    tempo(object["tempo"].toDouble()),
    time_signature(object["time_signature"].toInt()),
    track_href(object["track_href"].toString()),
    valence(object["valence"].toDouble())
{

}

}
