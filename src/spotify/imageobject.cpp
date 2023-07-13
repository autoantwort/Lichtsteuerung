#include "imageobject.h"

namespace Spotify::Objects {

ImageObject::ImageObject(const QJsonObject &object) : url(object["url"].toString()) {
    if (auto h = object.find("height"); h != object.end() && !h->isNull()) {
        height = h->toInt();
    }
    if (auto w = object.find("width"); w != object.end() && !w->isNull()) {
        width = w->toInt();
    }
}

ImageVector::ImageVector(const QJsonArray &array) {
    for (const auto &v : array) {
        emplace_back(v.toObject());
    }
}

} // namespace Spotify::Objects
