#ifndef USEROBJECT_H
#define USEROBJECT_H

#include <QObject>
#include <optional>
#include <QJsonObject>
#include <QVariant>

namespace Spotify::Objects{

/**
 * @brief The UserObject class https://developer.spotify.com/documentation/web-api/reference/users-profile/get-current-users-profile/#user-object-private
 */
class UserObject
{
    Q_GADGET
    Q_PROPERTY(QString email MEMBER email CONSTANT)
    Q_PROPERTY(QString id MEMBER id CONSTANT)
    Q_PROPERTY(QVariant userName READ getDisplayName)
public:
    /**
     * @brief display_name The name displayed on the user’s profile. null if not available.
     */
    const std::optional<QString> display_name;
    /**
     * @brief email The user’s email address, as entered by the user when creating their account. Important! This email address is unverified; there is no proof that it actually belongs to the user. This field is only available when the current user has granted access to the user-read-email scope.
     */
    const QString email;
    /**
     * @brief refreshToken The token you can use to get a new access token.
     */
    const QString refreshToken;
    /**
     * @brief id The Spotify user ID for the user.
     */
    const QString id;
public:
    UserObject() = default;
    UserObject(const QJsonObject & object);
    UserObject(const QJsonObject & object, const QString & refreshToken);
    void writeToJsonObject(QJsonObject & object)const;
    /**
     * @brief getDisplayName The display name(or username)
     * @return a variant, that is invalid if no display name exists, or that holds the display name
     */
    QVariant getDisplayName()const;
    /**
     * @brief login The function login this user and make this the current user, if the refresh_token is valid
     */
    Q_INVOKABLE void login()const;
    bool operator==(const UserObject & right){return id == right.id;}
};

}

#endif // USEROBJECT_H
