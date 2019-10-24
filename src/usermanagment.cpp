#include "usermanagment.h"
#include <QCryptographicHash>
#include <QJsonArray>

UserManagment::UserManagment() : defaultUser(new User(QStringLiteral("Default"), "")), currentUser(defaultUser.get()) {
#ifdef Q_OS_UNIX
    currentOsUserName = qgetenv("USER");
#else
    // Windows:
    currentOsUserName = qgetenv("UserName");
#endif
}

User *UserManagment::getUserById(ID::value_type id) {
    for (const auto &d : users) {
        if (d->getID() == id) {
            return d.get();
        }
    }
    return nullptr;
}

void UserManagment::addUser(const QString &name, const QString &password) {
    users.push_back(std::unique_ptr<User>(new User(name, QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256))));
}

bool UserManagment::removeUser(User *user, const QString &password) {
    const auto hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256);
    if (user->password == hash) {
        users.remove_if([=](const auto &p) { return p.get() == user; });
        return true;
    }
    for (const auto &u : users) {
        if (u->password == hash && u->havePermission(Admin)) {
            users.remove_if([=](const auto &p) { return p.get() == user; });
            return true;
        }
    }
    return false;
}

bool UserManagment::removeUser(User *user) {
    if (currentUser->havePermission(Admin) && user != currentUser) {
        users.remove_if([=](const auto &p) { return p.get() == user; });
        return true;
    }
    return false;
}

bool UserManagment::changeUserName(User *user, const QString &newName, const QString &password) {
    const auto hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256);
    if (user->password == hash) {
        user->setUsername(newName);
        return true;
    }
    for (const auto &u : users) {
        if (u->password == hash && u->havePermission(Admin)) {
            user->setUsername(newName);
            return true;
        }
    }
    return false;
}

bool UserManagment::changeUserPasswort(User *user, const QString &password, const QString &newPassword) {
    const auto hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256);
    if (user->password == hash) {
        user->password = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha3_256);
        return true;
    }
    return false;
}

bool UserManagment::login(User *user, const QString &password) {
    if (user == nullptr) {
        return false;
    }
    const auto hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256);
    if (user->password == hash) {
        currentUser = user;
        emit currentUserChanged();
        return true;
    }
    return false;
}

void UserManagment::autoLoginUser() {
    if (currentOsUserName.isEmpty()) {
        return;
    }
    for (const auto &user : users) {
        for (const auto &name : user->getAutoLoginUserNames()) {
            if (name == currentOsUserName) {
                currentUser = user.get();
                emit currentUserChanged();
                return;
            }
        }
    }
}

void UserManagment::logout(User *user) {
    if (currentUser == user) {
        currentUser = getDefaultUser();
        emit currentUserChanged();
    }
}
void UserManagment::logout() {
    currentUser = getDefaultUser();
    emit currentUserChanged();
}

// UserPermissionModel

QVariant UserPermissionModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 && index.row() >= rowCount(index)) {
        return QVariant("Index Out of Range");
    }
    const auto &me = UserManagment::metaEnum;
    switch (role) {
    case HavePermissionRole:
    case Qt::EditRole: return QVariant(user->havePermission(static_cast<UserManagment::Permission>(me.value(index.row()))));
    case Qt::DisplayRole:
    case PermissionNameRole: return QVariant(me.valueToKey(me.value(index.row())));
    default: return QVariant("Unknown role!");
    }
}

bool UserPermissionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == HavePermissionRole) {
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Admin)) {
            if (index.row() >= 0 && index.row() < rowCount(index)) {
                const auto &me = UserManagment::metaEnum;
                const auto p = static_cast<UserManagment::Permission>(me.value(index.row()));
                // ein admin darf sich nicht selber entmachten:
                if (p != UserManagment::Admin || UserManagment::get()->getCurrentUser() != user) {
                    if (user->havePermission(p) != value.toBool()) {
                        user->setPermission(p, value.toBool());
                        emit dataChanged(index, index, {HavePermissionRole});
                        return true;
                    }
                }
            }
        }
        // you cant change the setting, notify all bindings
        emit dataChanged(index, index, {HavePermissionRole});
    }
    return false;
}

// USER

void User::createUser(const QJsonObject &o) {
    if (o[QStringLiteral("password")].toString().length() != 0) { // we dont want a user without a password
        UserManagment::get()->users.push_back(std::unique_ptr<User>(new User(o)));
    }
}

User::User(const QJsonObject &o) : QObject(UserManagment::get()), id(o), username(o[QStringLiteral("username")].toString()), password(QByteArray::fromBase64(o[QStringLiteral("password")].toString().toLatin1())), permissionModel(this) {
    loadPermissions(o);
    const auto arrayNames = o[QStringLiteral("autologinUsernames")].toArray();
    for (const auto &i : arrayNames) {
        autologinUsernames.push_back(i.toString());
    }
}

void User::writeJsonObject(QJsonObject &o) const {
    o.insert(QStringLiteral("username"), username);
    o.insert(QStringLiteral("password"), QString::fromLatin1(password.toBase64()));
    QJsonArray a;
    for (auto permission : permissions) {
        a.push_back(permission);
    }
    o.insert(QStringLiteral("permissions"), a);
    QJsonArray names;
    for (const auto &i : autologinUsernames) {
        names.push_back(i);
    }
    o.insert(QStringLiteral("autologinUsernames"), names);
    IDBase::writeJsonObject(o);
}

User *UserManagment::getUserByName(const QString &name) const {
    for (const auto &u : users) {
        if (u->getUsername() == name) {
            return u.get();
        }
    }
    return nullptr;
}

void User::setUsername(const QString &u) {
    if (u == username) {
        return;
    }
    username = u;
    emit usernameChanged(username);
}

void User::setPermission(UserManagment::Permission p, bool get) {
    if (get) {
        if (permissions.find(p) == permissions.cend()) {
            permissions.insert(p);
            emit permissionChanged(p, true);
        }
    } else {
        if (permissions.find(p) != permissions.cend()) {
            permissions.erase(permissions.find(p));
            emit permissionChanged(p, false);
        }
    }
}

void User::loadPermissions(const QJsonObject &o) {
    const auto array = o[QStringLiteral("permissions")].toArray();
    for (const auto &i : array) {
        auto value = i.toInt(-1);
        if (value >= 0 && UserManagment::metaEnum.valueToKey(value)) {
            permissions.insert(static_cast<UserManagment::Permission>(value));
        }
    }
}
