#ifndef TIMEINTERVALOBJECT_H
#define TIMEINTERVALOBJECT_H

class QJsonObject;

namespace Spotify::Objects {

/**
 * @brief The TimeIntervalObject class https://developer.spotify.com/documentation/web-api/reference/tracks/get-audio-analysis/#time-interval-object
 */
class TimeIntervalObject {
public:
    /**
     * @brief start The starting point (in seconds) of the time interval.
     */
    const float start;
    /**
     * @brief duration The duration (in seconds) of the time interval.
     */
    const float duration;
    /**
     * @brief confidence The confidence, from 0.0 to 1.0, of the reliability of the interval.
     */
    const float confidence;

public:
    TimeIntervalObject(const QJsonObject &object);
};

} // namespace Spotify::Objects

#endif // TIMEINTERVALOBJECT_H
