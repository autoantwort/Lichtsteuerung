#include "programm.h"
#include <QJsonArray>


Programm::Programm(const QJsonObject &o):NamedObject(o),IDBase<Programm>(o)
{
    auto array = o["programms"].toArray();
    for(const auto r : array){
        const auto o = r.toObject();
        programms.emplace_back(IDBase<Device>::getIDBaseObjectByID(o["device"].toString().toLong()),
                IDBase<ProgrammPrototype>::getIDBaseObjectByID(o["programmPrototype"].toString().toLong()),
                o["offset"].toDouble());
    }
}

void Programm::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase<Programm>::writeJsonObject(o);
    QJsonArray array;
    for(const auto dp : programms){
        QJsonObject o;
        o.insert("offset",dp.offset);
        o.insert("device",QString::number(dp.device->getID().value()));
        o.insert("programmPrototype",QString::number(dp.getProgrammPrototyp()->getID().value()));
        array.append(o);
    }
    o.insert("programms",array);
}

void Programm::fill(unsigned char *data, size_t length, double time){
    // für jedes Programm
    for(const auto p : Programm::getAllIDBases()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(const auto dp : p->programms){

                //für jedes Channel Programm
                for(const auto cp : dp.getProgrammPrototyp()->getChannelProgramms()){
                    const auto channelNummer = dp.device->getStartDMXChannel() + cp.channel->getIndex();
                    if(channelNummer<length){
                        data[channelNummer] = cp.getValueForTime(time + dp.offset);
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
    for(const auto cp : programms){
        if(cp.device == device){
            return false;
        }
    }
    programms.emplace_back(device, programmPrototype, offset);
    return true;
}
