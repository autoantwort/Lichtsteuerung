#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <QObject>
#include <optional>
#include <QJsonObject>
#include <QJsonArray>
#include "modelvector.h"

namespace Spotify::Objects{

class ImageObject
{
    Q_GADGET
    Q_PROPERTY(QString url MEMBER url CONSTANT)
    /**
     * @brief height The image height in pixels. If unknown: null or not returned.
     */
    std::optional<int> height;
    /**
     * @brief height The image width in pixels. If unknown: null or not returned.
     */
    std::optional<int> width;
public:
    /**
     * @brief url The source URL of the image.
     */
    const QString url;
public:
    ImageObject(const QJsonObject &object);
    ImageObject() = default;
};

class ImageVector : public ModelVector<ImageObject>{
    Q_OBJECT
public:
    ImageVector() = default;
    ImageVector(const QJsonArray &array);
};

}

Q_DECLARE_METATYPE(Spotify::Objects::ImageObject);
Q_DECLARE_METATYPE(Spotify::Objects::ImageObject*);
Q_DECLARE_OPAQUE_POINTER(Spotify::Objects::ImageObject*)
#endif // IMAGEOBJECT_H
