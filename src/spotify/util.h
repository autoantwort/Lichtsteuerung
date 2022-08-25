#ifndef UTIL_H
#define UTIL_H

#include <QAbstractItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <optional>

namespace Spotify {

template <typename T>
std::optional<T> getOptional(const QJsonObject &object, const QString &key) {
    if (auto v = object.find(key); v != object.end()) {
        if constexpr (std::is_same_v<T, int>) {
            return v.value().toInt();
        } else if constexpr (std::is_same_v<T, bool>) {
            return v.value().toBool();
        } else if constexpr (std::is_floating_point_v<T>) {
            return v.value().toDouble();
        } else if constexpr (std::is_same_v<T, QString>) {
            return v.value().toString();
        } else if constexpr (std::is_base_of_v<QAbstractItemModel, T>) {
            return std::make_optional(T(v.value().toArray()));
        } else {
            return std::optional<T>(v.value().toObject());
        }
    }
    return std::nullopt;
}

} // namespace Spotify

#endif // UTIL_H
