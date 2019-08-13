#ifndef USERMANAGMENT_H
#define USERMANAGMENT_H

#include <QObject>
#include <QMetaEnum>
#include <vector>
#include "idbase.h"
#include <QJsonObject>
#include "modelvector.h"
#include <set>

class User;

/**
 * @brief The UserManagment class handles user, their permissions and passwords
 */
class UserManagment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(User* currentUser READ getCurrentUser NOTIFY currentUserChanged)
    Q_PROPERTY(User* defaultUser READ getCurrentUser CONSTANT)
    Q_PROPERTY(QAbstractItemModel * users READ getUserModel CONSTANT)
    Q_PROPERTY(QString currentOsUserName READ getCurrentOsUserName CONSTANT)
private:
    User * readUser;
    User * currentUser;
    QString currentOsUserName;
    ModelVector<std::unique_ptr<User>> users;
    UserManagment();
    friend class User;
public:
    ModelVector<std::unique_ptr<User>>& getUsers(){return users;}
    QAbstractItemModel* getUserModel(){return &users;}
    User * getUserById(const QJsonValue &id){return getUserById(id.toString().toLongLong());}
    User * getUserById(ID id){return getUserById(id.value());}
    User * getUserById(ID::value_type id);

    Q_INVOKABLE User * getDefaultUser()const{return readUser;}
    /**
     * @brief get Return the Singletone of the UserManagment
     * @return
     */
    static UserManagment * get(){static UserManagment m;return &m;}
    /**
     * @brief The Permission enum contains all Permissions a User can have
     */
    enum Permission{
        Admin=0, Read=1, Write=2,
        ADD_DEVICE=3,REMOVE_DEVICE=4,CHANGE_POSITION=5, CHANGE_NAME=6,CHANGE_DEVICE_DMX_CHANNEL=7,
        DEVICE_TAB=8,DEVICE_PROTOTYPE_TAB=9,PROGRAMM_PROTOTYPE_TAY=10,PROGRAMM_TAB=11,
        MOVE_CONTROL_ITEM=12, ADD_CONTROL_ITEM=13, CHANGE_GROUP_NAME=14, CHANGE_GROUP_DEVICES=15, CHANGE_MIN_MAX_MAPPING=16, CHANGE_TIMEOUTS=17, REMOVE_CONTROL_ITEM=18,
        SETTINGS_TAB=19, MODULES_TAB=20, MODULE_PROGRAMS_TAB=21,
        LAST_PERMISSION=22
    };
    Q_ENUM(Permission)
    /**
     * @brief metaEnum Is used to get the names of the entries in qml as string
     */
    const QMetaEnum metaEnum = QMetaEnum::fromType<Permission>();

    /**
      * @brief adds a user to the Usermanagment
      * @param name The Username of the new user
      * @param password The password of the new User
      */
    Q_INVOKABLE void addUser(const QString & name, const QString & password);


    /**
     * @brief removeUser Removes an user
     * @param user the user to remove
     * @param password from the admin
     * @return  true for success, false for failure
     */
    Q_INVOKABLE bool removeUser(User * user,const QString &password);
    /**
     * @brief removeUser Removes an user, only successfull if the current user is an admin
     * @param user the user to remove, not the current user
     * @return  true for success, false for failure
     */
    Q_INVOKABLE bool removeUser(User * user);
    /**
     * @brief changeUserName change the username of a user
     * @param user the User where to change the username
     * @param newName the new Username
     * @param password the password from the user or from an admin
     * @return  true for success, false for failure
     */
    Q_INVOKABLE bool changeUserName(User * user, const QString &newName,const QString &password);
    //Q_INVOKABLE bool changeUserPermission(User * user, Permission newPermission,const QString &password);
    /**
     * @brief changeUserPasswort change the passwort of a user
     * @param user the user where the password should be changed
     * @param password the current passwort of the user or of an admin
     * @param newPassword the new password of the user
     * @return true for success, false for failure
     */
    Q_INVOKABLE bool changeUserPasswort(User * user,const QString &password,const QString &newPassword);
    /**
     * @brief login logins a user and make this user the current user
     * @param user the user to login
     * @param passwort the password of the user
     * @return true for success, false for failure
     */
    Q_INVOKABLE bool login(User * user, const QString &passwort);
    /**
     * @brief autoLoginUser checks if the autologin user name of one user matches the currentOsUserName and login the user where the name match
     */
    void autoLoginUser();
    /**
     * @brief logout logout the current User
     */
    Q_INVOKABLE void logout();
    /**
     * @brief logout logouts the user user, if the user user is not logind, nothiing happens
     * @param user The user to logout
     */
    void logout(User * user);
    /**
     * @brief getUserByName Gets the first user with the given name
     * @param name The name to search for
     * @return An user with the given username or a nullptr if none found
     */
    User* getUserByName(const QString & name)const;
    /**
     * @brief getCurrentUser gets the current user that is log in
     * @return The currently logined user
     */
    User * getCurrentUser()const{return currentUser;}
    /**
     * @brief getCurrentOsUserName returns the username of the current os user that is logged in
     * @return the username of the currently logged in os user
     */
    QString getCurrentOsUserName()const{return currentOsUserName;}
signals:
    void currentUserChanged();
};

/**
 * @brief The UserPermissionModel class provides an QAbstractListModel implementation and lists all Permissions that exists and to every permission an bool that is true if the user have the spezific permission
 */
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

/**
 * @brief The User class represents a user. A user have a name and a password and have permissions
 */
class User : public QObject, public IDBase<User>{
    Q_OBJECT
    Q_PROPERTY(QString name READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QAbstractListModel * permissionModel READ getPermissionModel CONSTANT)
    Q_PROPERTY(QAbstractListModel * autologinUsernames READ getAutoLoginUserNameModel CONSTANT)
    friend class UserManagment;
    friend class UserPermissionModel;
private:
    QString username;
    QByteArray password;
    std::set<UserManagment::Permission> permissions;
    ModelVector<QString> autologinUsernames;
    UserPermissionModel permissionModel;
    void setUsername(const QString &u){if(u==username)return;username = u;emit usernameChanged(username);}
    void setPermission(UserManagment::Permission p,bool get = true);
    User(const QString & name, const QByteArray &password):username(name),password(password),permissionModel(this){}
    User(const QJsonObject &o);
public:
    static void createUser(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
    ~User(){UserManagment::get()->logout(this);}    
    QString getUsername()const{return username;}
    /**
     * @brief getPermissions Returns an set that contains all Permission that this user has
     * @return A set containing all Permission that this user has
     */
    std::set<UserManagment::Permission> getPermissions()const{return permissions;}
    /**
     * @brief havePermission check if a user have a spezific permission
     * @param p the permission to check
     * @return true is the user have the permission
     */
    Q_INVOKABLE bool havePermission(UserManagment::Permission p){return permissions.find(p)!=permissions.cend();}
    UserPermissionModel * getPermissionModel(){return &permissionModel;}
    QAbstractListModel * getAutoLoginUserNameModel(){return &autologinUsernames;}
    const ModelVector<QString>& getAutoLoginUserNames()const{return autologinUsernames;}
    Q_INVOKABLE void removeAutologinUsername(int index){
        if (index >= 0 && index < autologinUsernames.ssize()) {
            autologinUsernames.erase(index);
        }
    }
    Q_INVOKABLE void changeAutologinUsername(int index, const QString &newUserName){
        if (index >= 0 && index < autologinUsernames.ssize()) {
            autologinUsernames[index] = newUserName;
            autologinUsernames.dataChanged(index);
        }
    }
    Q_INVOKABLE void addAutologinUsername(const QString &newUserName){
        autologinUsernames.push_back(newUserName);
    }
signals:
    void usernameChanged(QString);
    void permissionChanged(UserManagment::Permission,bool);
};

#endif // USERMANAGMENT_H
