#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include "modelvector.h"
#include <QMetaType>
#include <optional>

class QJsonArray;

namespace Spotify::Objects {

class ImageObject {
    Q_GADGET
    /**
     * @brief height The image height in pixels. If unknown: null or not returned.
     */
    std::optional<int> height;
    /**
     * @brief height The image width in pixels. If unknown: null or not returned.
     */
    std::optional<int> width;
    /**
     * @brief url The source URL of the image.
     */
    QString url;

public:
    ImageObject(const QJsonObject &object);
    ImageObject() = default;
};

class ImageVector : public ModelVector<ImageObject> {
    Q_OBJECT
public:
    ImageVector() = default;
    ImageVector(const QJsonArray &array);
    virtual QVariant data(const QModelIndex &index, int role) const override {
        Q_UNUSED(role);
        if (index.row() >= 0 && index.row() < int(getVector().size())) {
            return QVariant::fromValue(&getVector()[index.row()]);
        }
        return QVariant();
    }
};

} // namespace Spotify::Objects

Q_DECLARE_METATYPE(Spotify::Objects::ImageObject);

#endif // IMAGEOBJECT_H
