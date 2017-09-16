#ifndef PROGRAMMPROTOTYPE_H
#define PROGRAMMPROTOTYPE_H
#include "idbase.h"
#include "deviceprototype.h"
#include <QEasingCurve>

class ProgrammPrototype : public IDBase<ProgrammPrototype>
{    
private:
    /**
     * @brief devicePrototype Für diesen Typ soll das Programm sein.
     */
    const DevicePrototype * devicePrototype;
    QString name;
    QString description;
    class TimePoint{
    public:
        const double time;
        mutable unsigned char value;
        mutable QEasingCurve easingCurveToNextPoint;
        TimePoint(double time,unsigned char value,QEasingCurve easingCurveToNextPoint):time(time),value(value),easingCurveToNextPoint(easingCurveToNextPoint){}
        bool operator < (const TimePoint & o)const{return time < o.time;}
        bool operator < (const double& o)const{return time < o;}
        friend bool operator < (const double& l, const TimePoint &r){return l < r.time;}
    };
    enum RepeatPolicy{Continue, Oscillate};
    class ChannelProgramm{
    public:
        const Channel * channel;
        RepeatPolicy repeatPolicy;
        std::set<TimePoint,std::less<>> timeline;
        unsigned char getValueForTime(double t)const;
        void changeTimeOfTimePoint(double time, double newTime);
    };
    std::vector<ChannelProgramm> programm;
public:
    ProgrammPrototype(const QJsonObject &o);
    ProgrammPrototype(ProgrammPrototype * devicePrototype);
    ChannelProgramm * getChannelProgramm(const Channel * channel);
    const std::vector<ChannelProgramm>& getChannelProgramms()const{return programm;}
};

#endif // PROGRAMMPROTOTYPE_H
