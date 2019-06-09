#ifndef CURRENTPLAYINGOBJECT_H
#define CURRENTPLAYINGOBJECT_H

#include <QObject>
#include <optional>
#include "trackobject.h"
#include <QJsonObject>

namespace Spotify::Objects{

//https://developer.spotify.com/documentation/web-api/reference/player/get-the-users-currently-playing-track/
class CurrentPlayingObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant progressMs READ getProgressAsVariant CONSTANT)
    Q_PROPERTY(QString currentlyPlayingType MEMBER currently_playing_type CONSTANT)
    Q_PROPERTY(bool isPlaying MEMBER is_playing CONSTANT)
    Q_PROPERTY(Objects::TrackObject_full *  currentTrack READ getCurrentTrackAsPointer CONSTANT)
public:
    /**
     * @brief timestamp Unix Millisecond Timestamp when data was updated at the spotify server
     */
    const long timestamp;
    /**
     * @brief receivedTimestamp timestamp that which was taken when data arrived from server
     */
    const long receivedTimestamp;
    /**
     * @brief progress_ms Progress into the currently playing track. Can be null.
     */
    const std::optional<int> progress_ms;
    /**
     * @brief is_playing If something is currently playing.
     */
    const bool is_playing;
    /**
     * @brief item The currently playing track. Can be null.
     */
    std::optional<TrackObject_full> & item;
    /**
     *  The object type of the currently playing item. Can be one of track, episode, ad or unknown.
     */
    const QString currently_playing_type;
public:
    CurrentPlayingObject(const QJsonObject &object, std::optional<TrackObject_full> & item, const long receivedTimestamp);
    CurrentPlayingObject() = default;
    /**
     * @brief getProgressInMs return the progress in ms at the current timestamp
     * @return the progress in ms or -1, if the Track is not playing
     */
    int getProgressInMs()const;
    /**
     * @brief getProgressAsVariant same as @see getProgressInMs, but instead of returning -1 for an error, this function returning a invalid variant
     */
    QVariant getProgressAsVariant()const;
    Objects::TrackObject_full  * getCurrentTrackAsPointer();
};

}

#endif // CURRENTPLAYINGOBJECT_H
