#ifndef PROGRAMMPROTOTYPE_H
#define PROGRAMMPROTOTYPE_H
#include "idbase.h"
#include "deviceprototype.h"
#include <QEasingCurve>

class TimePoint{
public:
    TimePoint(const QJsonObject &o);
    const double time;
    mutable unsigned char value;
    mutable QEasingCurve easingCurveToNextPoint;
    TimePoint(double time,unsigned char value,QEasingCurve easingCurveToNextPoint):time(time),value(value),easingCurveToNextPoint(easingCurveToNextPoint){}
    bool operator < (const TimePoint & o)const{return time < o.time;}
    bool operator < (const double& o)const{return time < o;}
    friend bool operator < (const double& l, const TimePoint &r){return l < r.time;}
    void writeJsonObject(QJsonObject &o)const;
};

class ProgrammPrototype : public NamedObject, public IDBase<ProgrammPrototype>
{    
    Q_OBJECT
    Q_PROPERTY(DevicePrototype* devicePrototype READ getDevicePrototype CONSTANT)
public:
    /**
     * @brief devicePrototype Für diesen Typ soll das Programm sein.
     */
    const DevicePrototype * devicePrototype;
    DevicePrototype * getDevicePrototype()const{return const_cast<DevicePrototype *>(devicePrototype);}
public:
    enum RepeatPolicy{Continue, Oscillate};
    class ChannelProgramm{
    public:
        ChannelProgramm(const QJsonObject &o);
        ChannelProgramm(const Channel * channel):channel(channel){}
        const Channel * channel;
        RepeatPolicy repeatPolicy;
        mutable std::set<TimePoint,std::less<>> timeline;
        unsigned char getValueForTime(double t)const;
        void changeTimeOfTimePoint(double time, double newTime);
        void writeJsonObject(QJsonObject &o)const;
    };
    std::vector<ChannelProgramm> programm;
public:
    ProgrammPrototype(const QJsonObject &o);
    ProgrammPrototype(DevicePrototype * devicePrototype, QString name, QString description="");
    ChannelProgramm * getChannelProgramm(const Channel * channel);
    ChannelProgramm * getChannelProgramm(int channelIndex);
    const std::vector<ChannelProgramm>& getChannelProgramms()const{return programm;}    
    void writeJsonObject(QJsonObject &o)const;
};

#endif // PROGRAMMPROTOTYPE_H
