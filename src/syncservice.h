#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include "id.h"
#include <QString>
#include <QJsonObject>
#include <map>
#include <tuple>
#include <vector>


/**
 * @brief The SyncService class has static methods to add Sync Messages and is an Interface to implement an real SyncService that have to implement UserManagment
 */
class SyncService {
public:
    // function definitions
    typedef void (*CreateMethod) (const QJsonObject &);
    typedef void (*UpdateMethod) (const ID &, const QString &,const QString &);
    typedef void (*RemoveMethod) (const ID &);
    typedef void (*CreateMemberMethod) (const ID &,const QString &,const QJsonObject &);
    typedef void (*RemoveMemberMethod) (const ID &,const QString &,const ID &);
private:
    // all active syncServices
    static std::vector<SyncService*> syncServices;
    // all registered classes
    static std::map<QString,std::tuple<CreateMethod,UpdateMethod,RemoveMethod,CreateMemberMethod,RemoveMemberMethod>> classes;
    enum MethodTupleIndex{CreateMethodIndex,UpdateMethodIndex,RemoveMethodIndex,CreateMemberMethodIndex,RemoveMemberMethodIndex};
protected:
    const std::map<QString,std::tuple<CreateMethod,UpdateMethod,RemoveMethod,CreateMemberMethod,RemoveMemberMethod>> & getRegisteredClasses()const{return classes;}
    // Methods for your own SyncService to Process messages
    void processCreateMessage(const QString &className, const QJsonObject &o);
    void processCreateMemberMessage(const QString &className,const QString &id,const QString &memberName, const QJsonObject &o);
    void processUpdateMessage(const QString &className, const QString &id, const QString &varName,const QString &varValue);
    void processRemoveMessage(const QString &className, const QString &id);
    void processRemoveMemberMessage(const QString &className,const QString &,const QString &memberName, const QString &id);
public:
    enum Operation{UPDATE,CREATE,REMOVE,UPDATE_MEMBER,CREATE_MEMBER,REMOVE_MEMBER};
    enum Access{READ,WRITE};
    static bool areSyncServicesActive(){return !syncServices.empty();}
    static void addCreateMessage(const QString &className, const QJsonObject &o);
    static void addCreateMemberMessage(const QString &className,const ID &,const QString &memberName, const QJsonObject &o);
    static void addUpdateMessage(const QString &className, const ID &id, const QString &varName,const QString &varValue);
    static void addRemoveMessage(const QString &className, const ID &id);
    static void addRemoveMemberMessage(const QString &className,const ID &,const QString &memberName, const ID &id);

    template<typename haveStaticFunctions>
    static void registerClass(const QString &name){registerClass(name,&haveStaticFunctions::create,&haveStaticFunctions::update,&haveStaticFunctions::remove,&haveStaticFunctions::createMember,&haveStaticFunctions::removeMember);}
    template<typename haveStaticFunctions>
    static void registerClassWithoutMember(const QString &name){registerClass(name,&haveStaticFunctions::create,&haveStaticFunctions::update,&haveStaticFunctions::remove);}
    template<typename haveStaticFunctions>
    static void registerClassWithoutMemberAndCreation(const QString &name){registerClass(name,nullptr,&haveStaticFunctions::update,nullptr);}
    static void registerClass(const QString &name,CreateMethod createMethod, UpdateMethod updateMethod,RemoveMethod removeMethod){registerClass(name,createMethod,updateMethod,removeMethod,nullptr,nullptr);}
    static void registerClass(const QString &name,CreateMethod createMethod, UpdateMethod updateMethod,RemoveMethod removeMethod, CreateMemberMethod createMemberMethod, RemoveMemberMethod removeMemberMethod){classes.insert(std::make_pair(name,std::make_tuple(createMethod,updateMethod,removeMethod,createMemberMethod,removeMemberMethod)));}

protected:
    /**
     * @brief enableSyncService you habe to enable your own syncService
     * @param b enable or disbale
     */
    void enableSyncService(bool b);
public:
    virtual void addCreateMessageImpl(const QString &className, const QJsonObject &o) = 0;
    virtual void addCreateMemberMessageImpl(const QString &className,const ID &,const QString &memberName, const QJsonObject &o) = 0;
    virtual void addUpdateMessageImpl(const QString &className, const ID &id, const QString &varName,const QString &varValue) = 0;
    virtual void addRemoveMessageImpl(const QString &className, const ID &id) = 0;
    virtual void addRemoveMemberMessageImpl(const QString &className,const ID &,const QString &memberName, const ID &id) = 0;
    virtual ~SyncService(){enableSyncService(false);}
};

#endif // SYNCSERVICE_H
