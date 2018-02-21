#include "usermanagment.h"
#include <QCryptographicHash>
#include <QJsonArray>

UserManagment::UserManagment():readUser(new User("Default","")),currentUser(readUser){
    /*auto pass = QCryptographicHash::hash(QString("12345").toUtf8(),QCryptographicHash::Sha3_256);
    qDebug()<<pass;
    auto admin = new User("Admin",pass);
    for(int i = 0 ; i< LAST_PERMISSION;++i){
        admin->setPermission(static_cast<Permission>(i));
    }*/
}

UserManagment::~UserManagment(){
    while (!IDBase<User>::getAllIDBases().empty()) {
        delete *IDBase<User>::getAllIDBases().cbegin();
    }
}



void UserManagment::addUser(const QString &name, const QString &password){
    new User(name,QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256));
}

bool UserManagment::removeUser(User *user, const QString &password){

    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        delete user;
        return true;
    }
    for(const auto u : IDBase<User>::getAllIDBases()){
        if(u->password==hash && u->havePermission(Admin)){
            delete u;
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
    for(const auto u : IDBase<User>::getAllIDBases()){
        if(u->password==hash && u->havePermission(Admin)){
            user->setUsername(newName);
            return true;
        }
    }
    return false;
}

/*
bool UserManagment::changeUserPermission(User *user, Permission newPermission, const QString &password){
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    for(const auto u : IDBase<User>::getAllIDBases()){
        if(u->password==hash && u->havePermission(Admin)){
            user->setPermission(newPermission);
            return true;
        }
    }
    return false;
}
*/

bool UserManagment::changeUserPasswort(User *user, const QString &password, const QString &newPassword){
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        user->password=QCryptographicHash::hash(newPassword.toUtf8(),QCryptographicHash::Sha3_256);
        return true;
    }
    return false;
}

bool UserManagment::login(User *user, const QString &password){
    if(user==nullptr){
        return false;
    }
    const auto hash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha3_256);
    if(user->password==hash){
        currentUser = user;
        emit currentUserChanged();
        return true;
    }
    return false;
}

void UserManagment::logout(User *user){
    if(currentUser==user){
        currentUser = readUser;
        emit currentUserChanged();
    }
}
void UserManagment::logout(){
    currentUser = readUser;
    emit currentUserChanged();
}

// UserPermissionModel

QVariant UserPermissionModel::data(const QModelIndex &index, int role) const{
    if(index.row()<0 && index.row()>=rowCount(index)){
        return QVariant("Index Out of Range");
    }
    switch (role) {
    case HavePermissionRole:
    case Qt::EditRole:
        return QVariant(user->havePermission(static_cast<UserManagment::Permission>(index.row())));
    case Qt::DisplayRole:
    case PermissionNameRole:
        return QVariant(UserManagment::get()->metaEnum.valueToKey(index.row()));
    default:
        return QVariant("Unknown role!");
    }
}

bool UserPermissionModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(role==HavePermissionRole){
        if(UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Admin)){
            if(index.row()>=0 && index.row()<rowCount(index)){
                const UserManagment::Permission p = static_cast<UserManagment::Permission>(index.row());
                if(user->havePermission(p)!=value.toBool()){
                    user->setPermission(p,value.toBool());
                    emit dataChanged(index,index,{HavePermissionRole});
                    return true;
                }
            }
        }
    }
    return false;
}


//USER

void User::createUser(const QJsonObject &o){
    if(o["password"].toString().length()!=0){// we dont want a user without a password
        new User(o);
    }
}

User::User(const QJsonObject &o):username(o["username"].toString()),password(QByteArray::fromBase64(o["password"].toString().toLatin1())),permissionModel(this){
    const auto array = o["permissions"].toArray();
    for(const auto & i : array){
        permissions.insert(static_cast<UserManagment::Permission>(i.toInt()));
    }
}

void User::writeJsonObject(QJsonObject &o) const{
    o.insert("username",username);
    o.insert("password",QString::fromLatin1(password.toBase64()));
    QJsonArray a;
    for(auto i = permissions.cbegin();i!=permissions.cend();++i){
        a.push_back(*i);
    }
    o.insert("permissions",a);
}

User * UserManagment::getUserByName(const QString &name) const{
    for(auto u = IDBase<User>::getAllIDBases().cbegin();u!=IDBase<User>::getAllIDBases().cend();++u){
        if((**u).getUsername()==name){
            return *u;
        }
    }
    return nullptr;
}


void User::setPermission(UserManagment::Permission p, bool get){
    if(get){
        if (permissions.find(p)==permissions.cend()) {
            permissions.insert(p);
            emit permissionChanged(p,true);
        }
    }else{
        if (permissions.find(p)!=permissions.cend()) {
            permissions.erase(permissions.find(p));
            emit permissionChanged(p,false);
        }
    }
}






