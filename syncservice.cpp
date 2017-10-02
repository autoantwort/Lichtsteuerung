#include "syncservice.h"


std::vector<SyncService*> SyncService::syncServices;
std::map<QString,std::tuple<SyncService::CreateMethod,SyncService::UpdateMethod,SyncService::RemoveMethod,SyncService::CreateMemberMethod,SyncService::RemoveMemberMethod>> SyncService::classes;

void SyncService::processCreateMessage(const QString &className, const QJsonObject &o){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(std::get<CreateMethodIndex>(c->second)){
            (*std::get<CreateMethodIndex>(c->second))(o);
        }
    }
}

void SyncService::processCreateMemberMessage(const QString &className, const QString &id, const QString &memberName, const QJsonObject &o){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(std::get<CreateMemberMethodIndex>(c->second)){
            (*std::get<CreateMemberMethodIndex>(c->second))(ID(id.toLong()),memberName,o);
        }
    }
}

void SyncService::processUpdateMessage(const QString &className, const QString &id, const QString &varName, const QString &varValue){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(std::get<UpdateMethodIndex>(c->second)){
            (*std::get<UpdateMethodIndex>(c->second))(ID(id.toLong()),varName,varValue);
        }
    }
}

void SyncService::processRemoveMemberMessage(const QString &className, const QString &id, const QString &memberName, const QString &memid){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(std::get<RemoveMemberMethodIndex>(c->second)){
            (*std::get<RemoveMemberMethodIndex>(c->second))(ID(id.toLong()),memberName,ID(memid.toLong()));
        }
    }
}

void SyncService::processRemoveMessage(const QString &className, const QString &id){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(std::get<RemoveMethodIndex>(c->second)){
            (*std::get<RemoveMethodIndex>(c->second))(ID(id.toLong()));
        }
    }
}

void SyncService::enableSyncService(bool b){
    // wenn true, soll er in die liste, sonst raus
    for(auto i = syncServices.cbegin();i!=syncServices.cend();++i){
        if(*i==this){ // gefunden
            if(b) // ist schon drin
                return;
            syncServices.erase(i); // soll raus
            return;
        }
    }
    if(b) // soll rein
        syncServices.push_back(this);
}

void SyncService::addCreateMemberMessage(const QString &className, const ID &id, const QString &memberName, const QJsonObject &o){
    for(const auto s : syncServices)
        s->addCreateMemberMessageImpl(className,id,memberName,o);
}
void SyncService::addCreateMessage(const QString &className, const QJsonObject &o){
    for(const auto s : syncServices)
        s->addCreateMessageImpl(className,o);
}
void SyncService::addUpdateMessage(const QString &className, const ID &id, const QString &varName, const QString &varValue){
    for(const auto s : syncServices)
        s->addUpdateMessageImpl(className,id,varName,varValue);
}
void SyncService::addRemoveMemberMessage(const QString &className, const ID &id, const QString &memberName, const ID &mid){
    for(const auto s : syncServices)
        s->addRemoveMemberMessageImpl(className,id,memberName,mid);
}
void SyncService::addRemoveMessage(const QString &className, const ID &id){
    for(const auto s : syncServices)
        s->addRemoveMessageImpl(className,id);
}
