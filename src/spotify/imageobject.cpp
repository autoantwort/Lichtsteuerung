#include "imageobject.h"

#include <QJsonArray>

namespace Spotify::Objects {

ImageObject::ImageObject(const QJsonObject &object) : url(object[QStringLiteral("url")].toString()) {
    if (auto h = object.find(QStringLiteral("height")); h != object.end() && !h->isNull()) {
        height = h->toInt();
    }
    if (auto w = object.find(QStringLiteral("width")); w != object.end() && !w->isNull()) {
        width = w->toInt();
    }
}

ImageVector::ImageVector(const QJsonArray &array) {
    for (const auto &v : array) {
        emplace_back(v.toObject());
    }
}

} // namespace Spotify::Objects
