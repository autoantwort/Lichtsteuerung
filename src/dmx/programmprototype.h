#ifndef PROGRAMMPROTOTYPE_H
#define PROGRAMMPROTOTYPE_H
#include "deviceprototype.h"
#include "idbase.h"
#include "modelvector.h"
#include <QEasingCurve>
#include <set>

namespace DMX {

/**
 * @brief The TimePoint class holds a time and a corresponding value and a QEasingCurve that describe how to interpolate to the next timepoint
 */
class TimePoint {
public:
    TimePoint(const QJsonObject &o);
    const double time;
    mutable unsigned char value;
    mutable QEasingCurve easingCurveToNextPoint;
    TimePoint(double time, unsigned char value, QEasingCurve easingCurveToNextPoint) : time(time), value(value), easingCurveToNextPoint(easingCurveToNextPoint) {}
    bool operator<(const TimePoint &o) const { return time < o.time; }
    bool operator<(const double &o) const { return time < o; }
    friend bool operator<(const double &l, const TimePoint &r) { return l < r.time; }
    void writeJsonObject(QJsonObject &o) const;
};

/**
 * @brief The RepeatPolicy enum describe what happens, when the time reach the end of the timeline
 */
enum RepeatPolicy { Continue, Oscillate };
/**
 * @brief The ChannelProgramm class holds a channel, the repeatPolicy and the Timeline
 */
class ChannelProgramm : public QObject {
    Q_OBJECT
    Q_ENUM(RepeatPolicy)
    Q_PROPERTY(RepeatPolicy repeatPolicy MEMBER repeatPolicy NOTIFY repeatPolicyChanged)
    Q_PROPERTY(Channel *channel READ getChannel CONSTANT)
private:
    RepeatPolicy repeatPolicy = Continue;

public:
    ChannelProgramm(const DevicePrototype *deviceProtype, const QJsonObject &o);
    ChannelProgramm(const Channel *channel) : channel(channel) { setParent(getChannel()); }
    const Channel *channel;
    Channel *getChannel() const { return const_cast<Channel *>(channel); }
    mutable std::set<TimePoint, std::less<>> timeline;
    unsigned char getValueForTime(double t) const;
    void changeTimeOfTimePoint(double time, double newTime);
    void writeJsonObject(QJsonObject &o) const;
signals:
    void repeatPolicyChanged();
};

class ChannelProgrammVector : public ModelVector<std::unique_ptr<ChannelProgramm>> {
    Q_OBJECT
};

/**
 * @brief The ProgrammPrototype class describe for a DevicePrototype for each Channel a ChannelProgramm
 */
class ProgrammPrototype : public NamedObject, public IDBase<ProgrammPrototype> {
    Q_OBJECT
    Q_PROPERTY(DevicePrototype *devicePrototype READ getDevicePrototype CONSTANT)
    Q_PROPERTY(QAbstractItemModel *channelProgramms READ getChannelProgrammModel CONSTANT)
public:
    static QString syncServiceClassName;
    /**
     * @brief devicePrototype Für diesen Typ soll das Programm sein.
     */
    const DevicePrototype *devicePrototype;
    DevicePrototype *getDevicePrototype() const { return const_cast<DevicePrototype *>(devicePrototype); }

private:
    ChannelProgrammVector programm;
private slots:
    void channelProgrammDestroyed(ChannelProgramm *c);

public:
    ProgrammPrototype(const QJsonObject &o);
    ProgrammPrototype(DevicePrototype *devicePrototype, QString name, QString description = "");
    ChannelProgramm *getChannelProgramm(const Channel *channel);
    ChannelProgramm *getChannelProgramm(int channelIndex);
    const std::vector<std::unique_ptr<ChannelProgramm>> &getChannelProgramms() const { return programm.getVector(); }
    std::vector<std::unique_ptr<ChannelProgramm>> &getChannelProgramms() { return programm.getVector(); }
    void writeJsonObject(QJsonObject &o) const;
    ChannelProgrammVector *getChannelProgrammModel() { return &programm; }
signals:
    void channelProgrammAdded(ChannelProgramm *);
    void channelProgrammRemoved(ChannelProgramm *);
private slots:
    void channelAdded(Channel *);
    void channelRemoved(Channel *);
};

} // namespace DMX

#endif // PROGRAMMPROTOTYPE_H
