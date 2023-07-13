#include "currentplayingobject.h"
#include "util.h"
#include <QDebug>
#include <chrono>

namespace Spotify::Objects {

CurrentPlayingObject::CurrentPlayingObject(const QJsonObject &object, const long receivedTimestamp)
    : timestamp(static_cast<long>(object["timestamp"].toDouble())), receivedTimestamp(receivedTimestamp), progress_ms(getOptional<int>(object, "progress_ms")),
      is_playing(object["is_playing"].toBool()), item(getOptional<TrackObject_full>(object, "item")), currently_playing_type(object["currently_playing_type"].toString()) {}

int CurrentPlayingObject::getProgressInMs() const {
    if (progress_ms) {
        using namespace std::chrono;
        const auto currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        // If the timestamp is the timestamp where the song starts, use this to calculate progress. More accurate then the progress_ms which have latency
        if (std::abs(currentTime - timestamp - *progress_ms) < 100) {
            return static_cast<int>(currentTime - timestamp);
        } else {
            return static_cast<int>(*progress_ms + currentTime - receivedTimestamp);
        }
    }
    return -1;
}

} // namespace Spotify::Objects
