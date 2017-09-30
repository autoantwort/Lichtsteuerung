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

enum RepeatPolicy{Continue, Oscillate};
class ChannelProgramm : public QObject{
    Q_OBJECT
    Q_ENUM(RepeatPolicy)
    Q_PROPERTY(RepeatPolicy repeatPolicy MEMBER repeatPolicy NOTIFY repeatPolicyChanged)
private:
    RepeatPolicy repeatPolicy;
public:
    ChannelProgramm(const QJsonObject &o);
    ChannelProgramm(const Channel * channel):channel(channel){setParent(getChannel());}
    const Channel * channel;
    Channel * getChannel()const{return const_cast<Channel*>(channel);}
    mutable std::set<TimePoint,std::less<>> timeline;
    unsigned char getValueForTime(double t)const;
    void changeTimeOfTimePoint(double time, double newTime);
    void writeJsonObject(QJsonObject &o)const;
signals:
    void repeatPolicyChanged();
};

class ProgrammPrototype : public NamedObject, public IDBase<ProgrammPrototype>
{    
    Q_OBJECT
    Q_PROPERTY(DevicePrototype* devicePrototype READ getDevicePrototype CONSTANT)
public:
    static QString syncServiceClassName;
    /**
     * @brief devicePrototype Für diesen Typ soll das Programm sein.
     */
    const DevicePrototype * devicePrototype;
    DevicePrototype * getDevicePrototype()const{return const_cast<DevicePrototype *>(devicePrototype);}
public:    
    std::vector<ChannelProgramm*> programm;
private slots:
    void channelProgrammDestroyed(ChannelProgramm * c);
public:
    ProgrammPrototype(const QJsonObject &o);
    ProgrammPrototype(DevicePrototype * devicePrototype, QString name, QString description="");
    ChannelProgramm * getChannelProgramm(const Channel * channel);
    ChannelProgramm * getChannelProgramm(int channelIndex);
    const std::vector<ChannelProgramm*>& getChannelProgramms()const{return programm;}
    void writeJsonObject(QJsonObject &o)const;
signals:
    void channelProgrammAdded(ChannelProgramm *);
    void channelProgrammRemoved(ChannelProgramm *);
private slots:
    void channelAdded(Channel *);
    void channelRemoved(Channel *);
};

#endif // PROGRAMMPROTOTYPE_H
