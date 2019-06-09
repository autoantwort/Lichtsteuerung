#include "albumobject.h"
#include "util.h"

namespace Spotify::Objects{

AlbumObject_simplified::AlbumObject_simplified(const QJsonObject& object): album_group(getOptional<QString>(object,"album_group")), album_type(object["album_type"].toString()),artists(object["artists"].toArray()),id(object["id"].toString()),images(object["images"].toArray()),name(object["name"].toString()),release_date(object["release_date"].toString())
{
}

}
