#include "artistobject.h"

#include <QJsonArray>
#include <QJsonObject>

namespace Spotify::Objects {

ArtistObject_simplified::ArtistObject_simplified(const QJsonObject &object)
    : href(object[QStringLiteral("href")].toString())
    , id(object[QStringLiteral("id")].toString())
    , name(object[QStringLiteral("name")].toString()) {}

ArtistVector::ArtistVector(const QJsonArray &array) {
    for (const auto &v : array) {
        emplace_back(v.toObject());
    }
}

} // namespace Spotify::Objects
