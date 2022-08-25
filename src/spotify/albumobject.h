#ifndef ALBUMOBJECT_H
#define ALBUMOBJECT_H

#include "artistobject.h"
#include "imageobject.h"
#include "modelvector.h"
#include <QJsonObject>
#include <QObject>
#include <optional>
namespace Spotify::Objects {

/**
 * @brief The AlbumObject_simplified class https://developer.spotify.com/documentation/web-api/reference/object-model/#album-object-simplified
 */
class AlbumObject_simplified {
    Q_GADGET
public:
    /**
     * @brief album_group The field is present when getting an artist’s albums. Possible values are “album”, “single”, “compilation”, “appears_on”. Compare to album_type this field represents
     * relationship between the artist and the album.
     */
    const std::optional<QString> album_group;
    /**
     * @brief album_type The type of the album: one of “album”, “single”, or “compilation”.
     */
    const QString album_type;
    /**
     * @brief artists The artists of the album. Each artist object includes a link in href to more detailed information about the artist.
     */
    const ArtistVector artists;
    /**
     * @brief id The [/documentation/web-api/#spotify-uris-and-ids) for the album.
     */
    const QString id;
    /**
     * @brief images The cover art for the album in various sizes, widest first.
     */
    const ImageVector images;
    /**
     * @brief name The name of the album. In case of an album takedown, the value may be an empty string.
     */
    const QString name;

public:
    AlbumObject_simplified(const QJsonObject &object);
};

} // namespace Spotify::Objects

#endif // ALBUMOBJECT_H
