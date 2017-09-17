#include "programmprototype.h"
#include <cmath>
#include <QJsonArray>



unsigned char ProgrammPrototype::ChannelProgramm::getValueForTime(double t)const{
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
    --iter;
    const TimePoint * left;
    double progress;
    if(iter == timeline.cend()){
        left = &*timeline.crbegin();
        progress = t / rightTime;
    }else{
        left = &*iter;
        progress = (t - left->time) / (rightTime - left->time);
    }
    return left->value + left->easingCurveToNextPoint.valueForProgress(progress) * (rightValue-left->value);
}

void ProgrammPrototype::ChannelProgramm::changeTimeOfTimePoint(double time, double newTime){
    auto i = timeline.find(time);
    if(i!=timeline.end()){
        auto curve = i->easingCurveToNextPoint;
        auto value = i->value;
        timeline.erase(i);
        timeline.insert(TimePoint(newTime, value, curve));
    }
}

void ProgrammPrototype::TimePoint::writeJsonObject(QJsonObject &o) const{
    o.insert("time",time);
    o.insert("value",value);
    o.insert("type",easingCurveToNextPoint.type());
    o.insert("amplitude",easingCurveToNextPoint.amplitude());
    o.insert("overshoot",easingCurveToNextPoint.overshoot());
    o.insert("period",easingCurveToNextPoint.period());
}

void ProgrammPrototype::ChannelProgramm::writeJsonObject(QJsonObject &o) const{
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
    o.insert("devicePrototype",QString::number(devicePrototype->getID().value()));
    o.insert("name",name);
    o.insert("description",description);
    QJsonArray array;
    for(const auto p : programm){
        QJsonObject o;
        p.writeJsonObject(o);
        array.append(o);
    }
    o.insert("programm",array);
}

ProgrammPrototype::TimePoint::TimePoint(const QJsonObject &o):time(o["time"].toDouble()),value(o["value"].toInt()),easingCurveToNextPoint(o["type"].toInt()){
    if(auto a = o.find("amplitude") != o.end()) easingCurveToNextPoint.setAmplitude(a->toDouble());
    if(auto a = o.find("overshoot") != o.end()) easingCurveToNextPoint.setOvershoot(a->toDouble());
    if(auto a = o.find("period")    != o.end()) easingCurveToNextPoint.setPeriod(a->toDouble());
}

ProgrammPrototype::ChannelProgramm::ChannelProgramm(const QJsonObject &o):IDBase<Channel>::getIDBaseObjectByID(o["channel"]),repeatPolicy(o["repeatPolicy"].toInt()){
    for(const auto p : o["timeline"].toArray()){
        timeline.emplace_back(p.toObject());
    }
}

ProgrammPrototype::ProgrammPrototype(const QJsonObject &o):IDBase(o),
    devicePrototype(IDBase<DevicePrototype>::getIDBaseObjectByID(o["devicePrototype"])),
    name(o["name"].toString()),
    description(o["description"].toString()){
    for(const auto p : o["programm"].toArray()){
        programm.emplace_back(p.toObject());
    }
}
