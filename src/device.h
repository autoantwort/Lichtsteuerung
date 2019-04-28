#ifndef DEVICE_H
#define DEVICE_H

#include "deviceprototype.h"
#include "QPoint"
#include "namedobject.h"
#include "dmxchannelfilter.h"



/**
 * @brief The Device class Ein "echtes" Gerät, zb der Laser oben rechts über der Bier Theke
 */
class Device : public NamedObject, public IDBase<Device>
{
    Q_OBJECT
    Q_PROPERTY(DevicePrototype * prototype READ getPrototype CONSTANT)
    Q_PROPERTY(unsigned int startDMXChannel READ getStartDMXChannel WRITE setStartDMXChannel NOTIFY startDMXChannelChanged)
    Q_PROPERTY(QPoint position READ getPosition WRITE setPosition NOTIFY positionChanged)
public:
    /**
     * @brief prototype Ein Pointer auf den Typ/Prototype, von dem das Gerät ist. (Ist es eine Lamp, ein Laser, .. ?)
     */
    const DevicePrototype * prototype;
    DevicePrototype * getPrototype()const{return const_cast<DevicePrototype *>(prototype);}
protected:
    /**
     * @brief startDMXChannel Der Start DMX Channel
     */
    unsigned int startDMXChannel;
    /**
     * @brief position Die Position des Geräts, es wird eine Karte geben, wo dann dieses Gerät mit diese Position eingezeichnet wird
     */
    QPoint position;
    /**
     * @brief filter Für jeden Channel einen Filter, der einen Default Value setzt und endergebnisse des Programms filtert;
     */
    std::vector<std::pair<Channel*,DMXChannelFilter*>> filter;
private slots:
    void channelRemoved(Channel *);
    void channelAdded(Channel *);
public:
    Device(const QJsonObject &o);

    Device(DevicePrototype * prototype, int startDMXChannel, QString name, QString desciption="",QPoint position = QPoint(-1,-1)):NamedObject(name,desciption),prototype(prototype),startDMXChannel(startDMXChannel),position(position){
        connect(prototype,&DevicePrototype::channelAdded,this,&Device::channelAdded);
        for(auto i = prototype->getChannels().cbegin();i != prototype->getChannels().cend();++i){
            filter.emplace_back(*i,new DMXChannelFilter);
        }
    }

    void writeJsonObject(QJsonObject &o)const;

    QPoint getPosition()const{return position;}
    Q_SLOT void setPosition(const QPoint &p){if(p == position)return;position = p;emit positionChanged(position);}

    unsigned int getStartDMXChannel()const{return startDMXChannel;}
    Q_SLOT void setStartDMXChannel(unsigned int newStart){if(newStart == startDMXChannel)return;startDMXChannel = newStart; emit startDMXChannelChanged(startDMXChannel);}

    const std::vector<Channel*> & getChannels()const{return prototype->getChannels();}

    const std::vector<std::pair<Channel*,DMXChannelFilter*>> & getChannelFilter()const{return filter;}

    Q_INVOKABLE DMXChannelFilter * getFilterForChannel( Channel * c);
    Q_INVOKABLE DMXChannelFilter * getFilterForChannelindex(int intdex);

signals:
    void startDMXChannelChanged(unsigned int newStartDMXChannel);
    void positionChanged(const QPoint & newPosition);
};

#endif // DEVICE_H
