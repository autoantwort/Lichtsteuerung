#include "syncservice.h"

void SyncService::processCreateMessage(const QString &className, const QJsonObject &o){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(c->second.get<CreateMethodIndex>()){
            *c->second.get<CreateMethodIndex>()(o);
        }
    }
}

void SyncService::processCreateMemberMessage(const QString &className, const QString &id, const QString &memberName, const QJsonObject &o){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(c->second.get<CreateMemberMethodIndex>()){
            *c->second.get<CreateMemberMethodIndex>()(ID(id.toLong()),memberName,o);
        }
    }
}

void SyncService::processUpdateMessage(const QString &className, const QString &id, const QString &varName, const QString &varValue){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(c->second.get<UpdateMethodIndex>()){
            *c->second.get<UpdateMethodIndex>()(ID(id.toLong()),varName,varValue);
        }
    }
}

void SyncService::processRemoveMemberMessage(const QString &className, const QString &id, const QString &memberName, const QString &memid){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(c->second.get<RemoveMemberMethodIndex>()){
            *c->second.get<RemoveMemberMethodIndex>()(ID(id.toLong()),memberName,ID(memid.toLong()));
        }
    }
}

void SyncService::processRemoveMessage(const QString &className, const QString &id){
    const auto c = classes.find(className);
    if(c!=classes.cend()){
        if(c->second.get<RemoveMethodIndex>()){
            *c->second.get<RemoveMethodIndex>()(ID(id.toLong()));
        }
    }
}

void SyncService::enableSyncService(bool b){
    // wenn true, soll er in die liste, sonst raus
    for(auto i = syncServices.cbegin();i!=syncServices.send();++i){
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
