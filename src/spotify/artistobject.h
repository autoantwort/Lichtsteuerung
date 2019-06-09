#ifndef ARTISTOBJECT_H
#define ARTISTOBJECT_H

#include <QObject>
#include "modelvector.h"
#include <QJsonObject>
#include <QJsonArray>

namespace Spotify::Objects{

/**
 * @brief The ArtistObject_simplified class https://developer.spotify.com/documentation/web-api/reference/object-model/#artist-object-simplified
 */
class ArtistObject_simplified
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString id MEMBER id CONSTANT)
public:
    /**
     * @brief href A link to the Web API endpoint providing full details of the artist.
     */
    const QString href;
    /**
     * @brief id The Spotify ID for the artist.
     */
    const QString id;
    /**
     * @brief name The name of the artist
     */
    const QString name;
public:
    ArtistObject_simplified(const QJsonObject &object);
    ArtistObject_simplified() = default;
};


class ArtistVector : public ModelVector<ArtistObject_simplified>{
    Q_OBJECT
public:
    ArtistVector() = default;
    ArtistVector(const QJsonArray & array);
};

}
Q_DECLARE_METATYPE(Spotify::Objects::ArtistObject_simplified)

#endif // ARTISTOBJECT_H
