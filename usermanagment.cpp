#include "usermanagment.h"
#include <QCryptographicHash>
#include <QJsonArray>

UserManagment::UserManagment(const QJsonObject &o){
    load(o);
}

void UserManagment::load(const QJsonObject &o){
    for(const auto o : o["user"].toArray()){
        user.push_back(new User(o.toObject()));
    }
}

void UserManagment::writeJsonObject(QJsonObject &o) const{
    QJsonArray a;
    for(const auto u : user){
        QJsonObject o;
        u->writeJsonObject(o);
        a.append(o);
    }
    o.insert("user",a);
}

UserManagment::UserManagment()
{

}

void UserManagment::addUser(const QString &name, const QString &password){
    user.push_back(new User(name,QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256)));
}

bool UserManagment::removeUser(User *user, const QString &password){
    const auto useriter = std::find(this->user.cbegin(),this->user.cend(),user);
    if(useriter==this->user.cend()){
        return false;
    }
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        this->user.erase(useriter);
        return true;
    }
    for(const auto u : this->user){
        if(u->password==hash && u->permission == Admin){
            this->user.erase(useriter);
            return true;
        }
    }
    return false;
}

bool UserManagment::changeUserName(User *user, const QString &newName, const QString &password){
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        user->setUsername(newName);
        return true;
    }
    for(const auto u : this->user){
        if(u->password==hash && u->permission == Admin){
            user->setUsername(newName);
            return true;
        }
    }
    return false;
}

bool UserManagment::changeUserPermission(User *user, Permission newPermission, const QString &password){
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    for(const auto u : this->user){
        if(u->password==hash && u->permission == Admin){
            user->setPermission(newPermission);
            return true;
        }
    }
    return false;
}

bool UserManagment::changeUserPasswort(User *user, const QString &password, const QString &newPassword){
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        user->password=QCryptographicHash::hash(newPassword.toUtf8(),QCryptographicHash::Sha3_256);
        return true;
    }
    return false;
}

User::User(const QJsonObject &o):username(o["username"].toString()),password(o["password"].toString().toLatin1()),permission(static_cast<UserManagment::Permission>(o["permission"].toInt())){

}

void User::writeJsonObject(QJsonObject &o) const{
    o.insert("username",username);
    o.insert("password",QString::fromLatin1(password.constData()));
    o.insert("permission",permission);
}

User * UserManagment::getUserByName(const QString &name) const{
    for(auto u = user.cbegin();u!=user.cend();++u){
        if((**u).getUsername()==name){
            return *u;
        }
    }
    return nullptr;
}







