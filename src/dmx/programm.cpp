#include "programm.h"
#include <QJsonArray>
#include <unordered_set>
#include "modelmanager.h"

namespace DMX{

TimeDistortion::TimeDistortion(const QJsonObject &o):enabled(o["enable"].toBool()),distortionCurve(QEasingCurve::Type(o["type"].toInt())),intervall(o["intervall"].toDouble()){

    const auto a = o.find("amplitude");
    if(a != o.end()) distortionCurve.setAmplitude(a->toDouble());
    const auto b = o.find("overshoot");
    if(b != o.end()) distortionCurve.setOvershoot(b->toDouble());
    const auto c = o.find("period");
    if(c != o.end()) distortionCurve.setPeriod(c->toDouble());
}

void TimeDistortion::writeJsonObject(QJsonObject &o) const{
    o.insert("enable",enabled);
    o.insert("intervall",intervall);
    o.insert("type",distortionCurve.type());
    o.insert("type",distortionCurve.type());
    o.insert("amplitude",distortionCurve.amplitude());
    o.insert("overshoot",distortionCurve.overshoot());
    o.insert("period",distortionCurve.period());
}

void TimeDistortion::setIntervall(double i){
    if(i==intervall)
        return;
    intervall=i;
    emit intervallChanged(i);
}

Programm::Programm(const QJsonObject &o):NamedObject(o),IDBase<Programm>(o),speed(o["speed"].toDouble()),timeDistortion(o["timeDistortion"].toObject())
{
    auto array = o["programms"].toArray();
    for(const auto r : array){
        const auto o = r.toObject();
        addDeviceProgramm(o);
    }
}

void Programm::addDeviceProgramm(const QJsonObject &o){
    programms.push_back(std::make_unique<DeviceProgramm>(ModelManager::get().getDeviceById(o["device"]),
                        ModelManager::get().getProgramPrototypeById(o["programmPrototype"]),
            o["offset"].toDouble()));
    emit deviceProgrammAdded(programms.back().get());
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
    o.insert("speed",speed);
    QJsonObject td;
    timeDistortion.writeJsonObject(td);
    o.insert("timeDistortion",td);
}

std::map<Programm*,std::map<Device*,std::vector<Channel*>>> Programm::run(){
    if (isRunning()) {
        return std::map<Programm*,std::map<Device*,std::vector<Channel*>>>();
    }
    std::unordered_set<int> newUsedChannels;
    for(const auto & p : this->programms){
        for(const auto & cp : p->getProgrammPrototyp()->getChannelProgramms()){
            const auto channelNummer = p->device->getStartDMXChannel() + cp->channel->getIndex();
            newUsedChannels.insert(channelNummer);
        }
    }
    std::map<Programm*,std::map<Device*,std::vector<Channel*>>> map;
    for(const auto & p : ModelManager::get().getPrograms()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(auto dp_ = p->programms.cbegin();dp_!=p->programms.cend();++dp_){
                auto & dp = *dp_;
                //für jedes Channel Programm
                for(const auto & cp : dp->getProgrammPrototyp()->getChannelProgramms()){
                    const auto channelNummer = dp->device->getStartDMXChannel() + cp->channel->getIndex();
                    if(newUsedChannels.find(channelNummer)!=newUsedChannels.end()){
                        map[p.get()][dp->getDevice()].push_back(cp->getChannel());
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
    for(const auto & p : ModelManager::get().getPrograms()){
        if(p->isRunning()){
            //für jedes Device Programm
            for(auto dp_ = p->programms.cbegin();dp_!=p->programms.cend();++dp_){
                auto & dp = *dp_;
                //für jedes Channel Programm
                for(const auto & cp : dp->getProgrammPrototyp()->getChannelProgramms()){
                    const auto channelNummer = dp->device->getStartDMXChannel() + cp->channel->getIndex();
                    if(channelNummer<length){
//#warning Fix distorstion:
                        // original : cause NAN : cp->getValueForTime(p->getTimeDistortion()->distort(time * p->getSpeed()) * dp->getSpeed()+ dp->getOffset())
                        data[channelNummer] = cp->getValueForTime((time * p->getSpeed()) * dp->getSpeed()+ dp->getOffset());
                    }
                }
            }
        }
    }
}

bool Programm::addDeviceProgramm(Device * device, ProgrammPrototype * programmPrototype, double offset){
    if(!device||!programmPrototype)
        return false;
    if(device->prototype != programmPrototype->devicePrototype){
        return false;
    }
    for(auto dp = programms.cbegin();dp!=programms.cend();++dp){
        if((**dp).device == device){
            return false;
        }
    }
    programms.push_back(std::make_unique<DeviceProgramm>(device, programmPrototype, offset));
    emit deviceProgrammAdded(programms.back().get());
    // if the device or program prototype gets deleted, we should delete the deviceProgram also
    connect(programmPrototype,&ProgrammPrototype::destroyed,this,&Programm::programPrototypeDeleted);
    connect(device,&Device::destroyed,this,&Programm::deviceDeleted);
    return true;
}

void Programm::removeDeviceProgramm(int index){
    if(static_cast<unsigned int>(index)<programms.size()){
        emit deviceProgrammRemoved(programms[index].get());
        programms.erase(programms.cbegin()+index);
    }
}

void Programm::programPrototypeDeleted(QObject * p){
    programms.remove_if([this,p](const auto & dp){
        if(dp->getProgrammPrototyp() == dynamic_cast<ProgrammPrototype*>(p)){
            emit deviceProgrammRemoved(dp.get());
            return true;
        }
        return false;
    });
}

void Programm::deviceDeleted(QObject * p){
    programms.remove_if([this,p](const auto & dp){
        if(dp->getDevice() == dynamic_cast<Device*>(p)){
            emit deviceProgrammRemoved(dp.get());
            return true;
        }
        return false;
    });
}

} // namespace DMX
