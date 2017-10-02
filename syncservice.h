#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include "id.h"
#include <QString>
#include <QJsonObject>
#include <map>
#include <tuple>
#include <vector>



class SyncService {
public:
    typedef void (*CreateMethod) (const QJsonObject &);
    typedef void (*UpdateMethod) (const ID &, const QString &,const QString &);
    typedef void (*RemoveMethod) (const ID &);
    typedef void (*CreateMemberMethod) (const ID &,const QString &,const QJsonObject &);
    typedef void (*RemoveMemberMethod) (const ID &,const QString &,const ID &);
private:
    static std::map<QString,std::tuple<CreateMethod,UpdateMethod,RemoveMethod,CreateMemberMethod,RemoveMemberMethod>> classes;
public:
    enum Operation{UPDATE,CREATE,REMOVE,UPDATE_MEMBER,CREATE_MEMBER,REMOVE_MEMBER};
    enum Access{READ,WRITE};
    static void addCreateMessage(const QString &className, const QJsonObject &o){}
    static void addCreateMemberMessage(const QString &className,const ID &,const QString &memberName, const QJsonObject &o){}
    static void addUpdateMessage(const QString &className, const ID &id, const QString &varName,const QString &varValue){}
    static void addRemoveMessage(const QString &className, const ID &id){}
    static void addRemoveMemberMessage(const QString &className,const ID &,const QString &memberName, const ID &id){}

    template<typename haveStaticFunctions>
    static void registerClass(const QString &name){registerClass(name,&haveStaticFunctions::create,&haveStaticFunctions::update,&haveStaticFunctions::remove,&haveStaticFunctions::createMember,&haveStaticFunctions::removeMember);}
    template<typename haveStaticFunctions>
    static void registerClassWithoutMember(const QString &name){registerClass(name,&haveStaticFunctions::create,&haveStaticFunctions::update,&haveStaticFunctions::remove);}
    template<typename haveStaticFunctions>
    static void registerClassWithoutMemberAndCreation(const QString &name){registerClass(name,nullptr,&haveStaticFunctions::update,nullptr);}
    static void registerClass(const QString &name,CreateMethod createMethod, UpdateMethod updateMethod,RemoveMethod removeMethod){registerClass(name,createMethod,updateMethod,removeMethod,nullptr,nullptr);}
    static void registerClass(const QString &name,CreateMethod createMethod, UpdateMethod updateMethod,RemoveMethod removeMethod, CreateMemberMethod createMemberMethod, RemoveMemberMethod removeMemberMethod){classes.insert(std::make_pair(name,std::make_tuple(createMethod,updateMethod,removeMethod,createMemberMethod,removeMemberMethod)));}
};

#endif // SYNCSERVICE_H
