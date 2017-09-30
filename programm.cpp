#include "programm.h"
#include <QJsonArray>
#include <unordered_set>


Programm::Programm(const QJsonObject &o):NamedObject(o,&syncServiceClassName),IDBase<Programm>(o)
{
    auto array = o["programms"].toArray();
    for(const auto r : array){
        const auto o = r.toObject();
        addDeviceProgramm(o);
    }
}

void Programm::addDeviceProgramm(const QJsonObject &o){
    programms.push_back(new DeviceProgramm(IDBase<Device>::getIDBaseObjectByID(o["device"].toString().toLong()),
            IDBase<ProgrammPrototype>::getIDBaseObjectByID(o["programmPrototype"].toString().toLong()),
            o["offset"].toDouble()));
    emit deviceProgrammAdded(programms.back());
    connect(programms.back(),&DeviceProgramm::destroyed,this,&Programm::deviceProgrammDeleted);
}

void Programm::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase<Programm>::writeJsonObject(o);
    QJsonArray array;
    for(auto dp = programms.cbegin();dp!=programms.cend();++dp){
        QJsonObject o;
        o.insert("offset",(**dp).getOffset());
        o.insert("device",QString::number((**dp).device->getID().value()));
        o.insert("programmPrototype",QString::number((**dp).getProgrammPrototyp()->getID().value()));
        array.append(o);
    }
    o.insert("programms",array);
}

std::map<Programm*,std::map<Device*,std::vector<Channel*>>> Programm::run(){
    if (isRunning()) {
        return std::map<Programm*,std::map<Device*,std::vector<Channel*>>>();
    }
    std::unordered_set<int> newUsedChannels;
    for(const auto p : this->programms){
        for(const auto cp : p->getProgrammPrototyp()->getChannelProgramms()){
            const auto channelNummer = p->device->getStartDMXChannel() + cp->channel->getIndex();
            newUsedChannels.insert(channelNummer);
        }
    }
    std::map<Programm*,std::map<Device*,std::vector<Channel*>>> map;
    for(const auto p : Programm::getAllIDBases()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(auto dp_ = p->programms.cbegin();dp_!=p->programms.cend();++dp_){
                auto dp = *dp_;
                //für jedes Channel Programm
                for(const auto cp : dp->getProgrammPrototyp()->getChannelProgramms()){
                    const auto channelNummer = dp->device->getStartDMXChannel() + cp->channel->getIndex();
                    if(newUsedChannels.find(channelNummer)!=newUsedChannels.end()){
                        map[p][dp->getDevice()].push_back(cp->getChannel());
                    }
                }
            }
        }
    }
    if(map.empty())
        setRunning(true);
    return map;
}

void Programm::fill(unsigned char *data, size_t length, double time){
    // für jedes Programm
    for(const auto p : Programm::getAllIDBases()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(auto dp_ = p->programms.cbegin();dp_!=p->programms.cend();++dp_){
                auto dp = *dp_;
                //für jedes Channel Programm
                for(const auto cp : dp->getProgrammPrototyp()->getChannelProgramms()){
                    const auto channelNummer = dp->device->getStartDMXChannel() + cp->channel->getIndex();
                    if(channelNummer<length){
                        data[channelNummer] = cp->getValueForTime(p->getTimeDistortion()->distort(time * p->getSpeed()) * dp->getSpeed()+ dp->getOffset());
                    }
                }
            }
        }
    }
}

bool Programm::addDeviceProgramm(Device * device, ProgrammPrototype * programmPrototype, double offset){
    if(device->prototype != programmPrototype->devicePrototype){
        return false;
    }
    for(auto dp = programms.cbegin();dp!=programms.cend();++dp){
        if((**dp).device == device){
            return false;
        }
    }
    auto newDeviceProgramm = new DeviceProgramm(device, programmPrototype, offset);
    programms.push_back(newDeviceProgramm);
    emit deviceProgrammAdded(newDeviceProgramm);
    connect(newDeviceProgramm,&DeviceProgramm::destroyed,this,&Programm::deviceProgrammDeleted);
    return true;
}

void Programm::deviceProgrammDeleted(QObject *d){
    for(auto i = programms.cbegin();i!=programms.cend();++i){
        if(*i == d){
            programms.erase(i);
            emit deviceProgrammRemoved(qobject_cast<DeviceProgramm*>(d));
            return;
        }
    }
}
