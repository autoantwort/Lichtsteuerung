#ifndef TRACKOBJECT_H
#define TRACKOBJECT_H

#include "albumobject.h"
#include "artistobject.h"
#include <QJsonObject>
#include <QObject>

namespace Spotify::Objects {

// https://developer.spotify.com/documentation/web-api/reference/object-model/#track-object-full
class TrackObject_full {
    Q_GADGET
public:
    /**
     * @brief album The album on which the track appears. The album object includes a link in href to full information about the album.
     */
    const AlbumObject_simplified album;
    /**
     * @brief artists The artists who performed the track. Each artist object includes a link in href to more detailed information about the artist.
     */
    const ArtistVector artists;
    /**
     * @brief duration_ms The track length in milliseconds.
     */
    const int duration_ms;
    /**
     * @brief id The Spotify ID for the track.
     */
    const QString id;
    /**
     * @brief name The name of the track.
     */
    const QString name;
    /**
     * @brief popularity The popularity of the track. The value will be between 0 and 100, with 100 being the most popular.
     * The popularity of a track is a value between 0 and 100, with 100 being the most popular. The popularity is calculated by algorithm and is based, in the most part, on the total number of plays
     * the track has had and how recent those plays are. Generally speaking, songs that are being played a lot now will have a higher popularity than songs that were played a lot in the past.
     * Duplicate tracks (e.g. the same track from a single and an album) are rated independently. Artist and album popularity is derived mathematically from track popularity. Note that the popularity
     * value may lag actual popularity by a few days: the value is not updated in real time.
     */
    const int popularity;

public:
    TrackObject_full(const QJsonObject &object);
};

} // namespace Spotify::Objects

#endif // TRACKOBJECT_H
