#include "programmprototype.h"
#include <cmath>
#include <QJsonArray>

namespace DMX{

ProgrammPrototype::ProgrammPrototype(DevicePrototype *devicePrototype, QString name, QString description):NamedObject(name,description),devicePrototype(devicePrototype){
    setParent(devicePrototype);
    connect(devicePrototype,&DevicePrototype::channelAdded,this,&ProgrammPrototype::channelAdded);
    connect(devicePrototype,&DevicePrototype::channelRemoved,this,&ProgrammPrototype::channelRemoved);
    programm.beginPushBack(devicePrototype->getChannels().size());
    for(const auto channel : devicePrototype->getChannels()){
        programm.getChannelProgramms().push_back(new ChannelProgramm(channel));
        emit channelProgrammAdded(programm.getChannelProgramms().back());
    }
    programm.endPushBack();
}

void ProgrammPrototype::channelProgrammDestroyed(ChannelProgramm * c){
    for(auto i = programm.getChannelProgramms().cbegin();i!=programm.getChannelProgramms().cend();++i){
        if(*i==c){
            programm.erase(i);
            break;
        }
    }
    emit channelProgrammRemoved(c);
}

void ProgrammPrototype::channelAdded(Channel *c){
    programm.beginPushBack(1);
    programm.getChannelProgramms().push_back(new ChannelProgramm(c));
    programm.endPushBack();
    emit channelProgrammAdded(programm.getChannelProgramms().back());
}
void ProgrammPrototype::channelRemoved(Channel *c){
    for(auto i = programm.getChannelProgramms().cbegin();i!=programm.getChannelProgramms().cend();++i){
        if((**i).getChannel()==c){
            auto cp = *i;
            programm.erase(i);
            emit channelProgrammRemoved(cp);
            delete cp;
            return;
        }
    }
}


unsigned char ChannelProgramm::getValueForTime(double t)const{
    // Kein Punkt in der Timeline, 0 wird zurück gegeben
    if (timeline.empty()) {
        return 0;
    }
    // Bei nur einem Punkt ist der Value die ganze zeit konstant
    if(timeline.size()==1){
        return timeline.begin()->value;
    }
    // Da t größer als die Länge des Programms wird, t richtig kürzen
    auto length = timeline.rbegin()->time;
    if(repeatPolicy == Continue){
        t = fmod(t,length);
    }else{ // Oscillate
        bool ungrade = int(t / length)&1;
        t = fmod(t,length);
        if(ungrade){
            t = length - t;
        }
    }
    // den rechten und linken Zeitpunkt bestimmen und interpolieren.
    auto iter = timeline.upper_bound(t);
    const auto rightValue = iter->value;
    const auto rightTime = iter->time;

    const TimePoint * left;
    double progress;
    if(iter == timeline.cbegin()){
        left = &*timeline.crbegin();
        progress = t / rightTime;
    }else{
        --iter;
        left = &*iter;
        progress = (t - left->time) / (rightTime - left->time);
    }
    return left->value + left->easingCurveToNextPoint.valueForProgress(progress) * (rightValue-left->value);
}

void ChannelProgramm::changeTimeOfTimePoint(double time, double newTime){
    auto i = timeline.find(time);
    if(i!=timeline.end()){
        auto curve = i->easingCurveToNextPoint;
        auto value = i->value;
        timeline.erase(i);
        timeline.insert(TimePoint(newTime, value, curve));
    }
}

void TimePoint::writeJsonObject(QJsonObject &o) const{
    o.insert("time",time);
    o.insert("value",value);
    o.insert("type",easingCurveToNextPoint.type());
    o.insert("amplitude",easingCurveToNextPoint.amplitude());
    o.insert("overshoot",easingCurveToNextPoint.overshoot());
    o.insert("period",easingCurveToNextPoint.period());
}

void ChannelProgramm::writeJsonObject(QJsonObject &o) const{
    o.insert("channel",QString::number(channel->getID().value()));
    o.insert("repeatPolicy",repeatPolicy);
    QJsonArray array;
    for(const auto p : timeline){
        QJsonObject o;
        p.writeJsonObject(o);
        array.append(o);
    }
    o.insert("timeline",array);
}

void ProgrammPrototype::writeJsonObject(QJsonObject &o) const{
    IDBase<ProgrammPrototype>::writeJsonObject(o);
    NamedObject::writeJsonObject(o);
    o.insert("devicePrototype",QString::number(devicePrototype->getID().value()));
    QJsonArray array;
    for(const auto p : programm.getChannelProgramms()){
        QJsonObject o;
        p->writeJsonObject(o);
        array.append(o);
    }
    o.insert("programm",array);
}

TimePoint::TimePoint(const QJsonObject &o):time(o["time"].toDouble()),value(o["value"].toInt()),easingCurveToNextPoint(QEasingCurve::Type(o["type"].toInt())){
    const auto a = o.find("amplitude");
    if(a != o.end()) easingCurveToNextPoint.setAmplitude(a->toDouble());
    const auto b = o.find("overshoot");
    if(b != o.end()) easingCurveToNextPoint.setOvershoot(b->toDouble());
    const auto c = o.find("period");
    if(c != o.end()) easingCurveToNextPoint.setPeriod(c->toDouble());
}

ChannelProgramm::ChannelProgramm(const QJsonObject &o):repeatPolicy(RepeatPolicy(o["repeatPolicy"].toInt())),channel(IDBase<Channel>::getIDBaseObjectByID(o["channel"])){
    setParent(getChannel());
    for(const auto p : o["timeline"].toArray()){
        timeline.insert(p.toObject());
    }
}

ProgrammPrototype::ProgrammPrototype(const QJsonObject &o):NamedObject(o),IDBase<ProgrammPrototype>(o),
    devicePrototype(IDBase<DevicePrototype>::getIDBaseObjectByID(o["devicePrototype"])){
    setParent(getDevicePrototype());
    connect(devicePrototype,&DevicePrototype::channelAdded,this,&ProgrammPrototype::channelAdded);
    connect(devicePrototype,&DevicePrototype::channelRemoved,this,&ProgrammPrototype::channelRemoved);
    for(const auto p : o["programm"].toArray()){
        programm.getChannelProgramms().push_back(new ChannelProgramm(p.toObject()));
        emit channelProgrammAdded(programm.getChannelProgramms().back());
    }
}

ChannelProgramm * ProgrammPrototype::getChannelProgramm(const Channel * channel){
    for(auto p = programm.getChannelProgramms().begin() ; p != programm.getChannelProgramms().end() ;++p){
        if((**p).channel == channel)
            return *p;
    }
    return nullptr;
}
ChannelProgramm * ProgrammPrototype::getChannelProgramm(int index){
    for(auto p = programm.getChannelProgramms().begin() ; p != programm.getChannelProgramms().end() ;++p){
        if((**p).channel->getIndex() == index)
            return *p;
    }
    return nullptr;
}

} // namespace DMX
