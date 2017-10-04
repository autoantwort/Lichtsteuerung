#ifndef USERMANAGMENT_H
#define USERMANAGMENT_H

#include <QObject>
#include <vector>
#include "idbase.h"
#include <QJsonObject>

class User;

class UserManagment : public QObject
{
    Q_OBJECT
private:
    std::vector<User*> user;
public:
    static UserManagment * get(){static UserManagment m;return &m;}
    enum Permission{Admin, Read, Write};
    UserManagment();
    UserManagment(const QJsonObject &o);
    void load(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
    void addUser(User * user);
    Q_INVOKABLE void addUser(const QString & name, const QString & password);
    Q_INVOKABLE bool removeUser(User * user,const QString &password);
    Q_INVOKABLE bool changeUserName(User * user, const QString &newName,const QString &password);
    Q_INVOKABLE bool changeUserPermission(User * user, Permission newPermission,const QString &password);
    Q_INVOKABLE bool changeUserPasswort(User * user,const QString &password,const QString &newPassword);
    const std::vector<User*>& getUser()const{return user;}
};


class User : public QObject, public IDBase<User>{
    Q_OBJECT
    Q_PROPERTY(QString name READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(UserManagment::Permission permission READ getPermission NOTIFY permissionChanged)
    friend class UserManagment;
private:
    QString username;
    QByteArray password;
    UserManagment::Permission permission = UserManagment::Read;
    void setUsername(const QString &u){if(u==username)return;username = u;emit usernameChanged(username);SyncService::addUpdateMessage("User",getID(),"username",username);}
    void setPermission(UserManagment::Permission p){if(p==permission)return;permission = p;emit permissionChanged();SyncService::addUpdateMessage("User",getID(),"permission",QString::number(permission));}
    User(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
public:
    User(const QString & name, const QByteArray &password):username(name),password(password){}
    QString getUsername()const{return username;}
    UserManagment::Permission getPermission()const{return permission;}
signals:
    void usernameChanged(QString);
    void permissionChanged();
};

#endif // USERMANAGMENT_H
