#ifndef USERMANAGMENT_H
#define USERMANAGMENT_H

#include <QObject>
#include <vector>
#include "idbase.h"
#include <QJsonObject>

class User;

/**
 * @brief The UserManagment class handles user, their permissions and passwords
 */
class UserManagment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(User* currentUser READ getCurrentUser NOTIFY currentUserChanged)
    Q_PROPERTY(User* defaultUser READ getCurrentUser CONSTANT)
private:
    User * readUser;
    User * currentUser;
public:
    Q_INVOKABLE User * getDefaultUser()const{return readUser;}
    static UserManagment * get(){static UserManagment m;return &m;}
    enum Permission{
        Admin, Read, Write,
        ADD_DEVICE,REMOVE_DEVICE,CHANGE_POSITION, CHANGE_NAME,CHANGE_DEVICE_DMX_CHANNEL,
        DEVICE_TAB,DEVICE_PROTOTYPE_TAB,PROGRAMM_PROTOTYPE_TAY,PROGRAMM_TAB,
        MOVE_CONTROL_ITEM, ADD_CONTROL_ITEM, CHANGE_GROUP_NAME, CHANGE_GROUP_DEVICES, CHANGE_MIN_MAX_MAPPING, CHANGE_TIMEOUTS,

        LAST_PERMISSION
    };
    Q_ENUM(Permission)
    const QMetaEnum metaEnum = QMetaEnum::fromType<Permission>();
    UserManagment();
    Q_INVOKABLE void addUser(const QString & name, const QString & password);
    Q_INVOKABLE bool removeUser(User * user,const QString &password);
    Q_INVOKABLE bool changeUserName(User * user, const QString &newName,const QString &password);
    Q_INVOKABLE bool changeUserPermission(User * user, Permission newPermission,const QString &password);
    Q_INVOKABLE bool changeUserPasswort(User * user,const QString &password,const QString &newPassword);
    Q_INVOKABLE bool login(User * user, const QString &passwort);
    Q_INVOKABLE void logout();
    void logout(User * user);
    User* getUserByName(const QString & name)const;
    User * getCurrentUser()const{return currentUser;}
signals:
    void currentUserChanged();
};

class UserPermissionModel : public QAbstractListModel{
    Q_OBJECT
    User* user;
public:
    UserPermissionModel(User * user):user(user){}
    enum GroupModelRoles{
        HavePermissionRole = Qt::UserRole+1,
        PermissionNameRole
    };

    virtual int rowCount(const QModelIndex & = QModelIndex())const override {return UserManagment::LAST_PERMISSION;}
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override{
        auto r = QAbstractListModel::flags(index);
        r.setFlag(Qt::ItemIsEditable);
        return r;
    }
    QHash<int,QByteArray> roleNames() const override{
        auto r = QAbstractListModel::roleNames();
        r[HavePermissionRole] = "havePermission";
        r[PermissionNameRole] = "permissionName";
        return r;
    }
};

class User : public QObject, public IDBase<User>{
    Q_OBJECT
    Q_PROPERTY(QString name READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QAbstractListModel * permissionModel READ getPermissionModel CONSTANT)
    friend class UserManagment;
    friend class UserPermissionModel;
private:
    QString username;
    QByteArray password;
    std::set<UserManagment::Permission> permissions;
    UserPermissionModel permissionModel;
    void setUsername(const QString &u){if(u==username)return;username = u;emit usernameChanged(username);SyncService::addUpdateMessage("User",getID(),"username",username);}
    void setPermission(UserManagment::Permission p,bool get = true);
    User(const QString & name, const QByteArray &password):username(name),password(password),permissionModel(this){}
    User(const QJsonObject &o);
public:
    static void createUser(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
    ~User(){UserManagment::get()->logout(this);}
    QString getUsername()const{return username;}
    std::set<UserManagment::Permission> getPermissions()const{return permissions;}
    Q_INVOKABLE bool havePermission(UserManagment::Permission p){return permissions.find(p)!=permissions.cend();}
    UserPermissionModel * getPermissionModel(){return &permissionModel;}
signals:
    void usernameChanged(QString);
    void permissionChanged(UserManagment::Permission,bool);
};

#endif // USERMANAGMENT_H
