#include "programmprototype.h"
#include <cmath>



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
