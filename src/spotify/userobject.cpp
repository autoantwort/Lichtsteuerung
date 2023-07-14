#include "userobject.h"
#include "spotify.h"
#include "util.h"
#include <QDebug>

namespace Spotify::Objects {

UserObject::UserObject(const QJsonObject &object)
    : display_name(getOptional<QString>(object, "display_name"))
    , email(object["email"].toString())
    , refreshToken(object["refreshToken"].toString())
    , id(object["id"].toString()) {}
UserObject::UserObject(const QJsonObject &object, const QString &refreshToken)
    : display_name(getOptional<QString>(object, "display_name"))
    , email(object["email"].toString())
    , refreshToken(refreshToken)
    , id(object["id"].toString()) {}

void UserObject::writeToJsonObject(QJsonObject &object) const {
    if (display_name) {
        object["display_name"] = *display_name;
    }
    object["refreshToken"] = refreshToken;
    object["email"] = email;
    object["id"] = id;
}

QVariant UserObject::getDisplayName() const {
    if (display_name) return *display_name;
    return {};
}

void UserObject::login() const {
    get().loginUser(*this);
}

} // namespace Spotify::Objects
