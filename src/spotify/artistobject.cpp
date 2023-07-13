#include "artistobject.h"

namespace Spotify::Objects {

ArtistObject_simplified::ArtistObject_simplified(const QJsonObject &object) : href(object["href"].toString()), id(object["id"].toString()), name(object["name"].toString()) {}

ArtistVector::ArtistVector(const QJsonArray &array) {
    for (const auto &v : array) {
        emplace_back(v.toObject());
    }
}

} // namespace Spotify::Objects
