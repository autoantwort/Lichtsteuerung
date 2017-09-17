#include "programm.h"
#include <QJsonArray>


Programm::Programm(const QJsonObject &o):IDBase(o),name(o["name"].toString()),description(o["description"].toString())
{
    auto array = o["programms"].toArray();
    for(const auto r : array){
        const auto o = r.toObject();
        programms.emplace_back(o["offset"].toDouble(),
                IDBase<Device>::getIDBaseObjectByID(o["device"].toString().toLong()),
                IDBase<ProgrammPrototype>::getIDBaseObjectByID(o["programmPrototype"].toString().toLong()));
    }
}

void Programm::writeJsonObject(QJsonObject &o) const{
    IDBase<Programm>::writeJsonObject(o);
    o.insert("name",name);
    o.insert("description",description);
    QJsonArray array;
    for(const auto dp : programms){
        QJsonObject o;
        o.insert("offset",dp.offset);
        o.insert("device",QString::number(dp.device->getID().value()));
        o.insert("programmPrototype",QString::number(dp.programmPrototype->getID().value()));
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
                for(const auto cp : dp.programmPrototype->getChannelProgramms()){
                    const auto channelNummer = dp.device->getStartDMXChannel() + cp.channel->getIndex();
                    if(channelNummer<length){
                        data[channelNummer] = cp.getValueForTime(time + dp.offset);
                    }
                }
            }
        }
    }
}
