#ifndef DEVICEPROTOTYPE_H
#define DEVICEPROTOTYPE_H

#include <QString>
#include <vector>
#include <QVector>
#include "channel.h"
#include "idbase.h"
#include <QAbstractListModel>
#include <QDebug>
#include <memory>
#include "modelvector.h"

namespace DMX{

class ChannelVector : public ModelVector<std::unique_ptr<Channel>>{
    Q_OBJECT
};

/** Jedes Gerät(Scanner/Laser/Lampe/...) bekommt ein Device Prototype, wo festgelegt wird, auf welchem Channel welche Daten anliegen
 * @brief The DevicePrototype class
 */
class DevicePrototype : public NamedObject, public IDBase<DevicePrototype>
{
    Q_OBJECT
    Q_PROPERTY(int numberOfChannels READ getNumberOfChannels NOTIFY numberOfChannelsChanged)
    Q_PROPERTY(QAbstractItemModel * channel READ getChannelModel CONSTANT)
private:
    /**
     * @brief channels Ein vector die die einzelnen Channel Informationen hält
     */
    ChannelVector channels;
public:
    DevicePrototype(const QJsonObject &o);
    DevicePrototype(QString name, QString description=""):NamedObject(name,description){}
    int getNumberOfChannels()const{return static_cast<int>(channels.getVector().size());}
    /**
     * @brief removeChannels Entfernt Channel bis zu einem bestimmten Index
     * @param newMaxIndex Der neue Maximale Index (Inclusiv)
     */
    void removeChannels(int newMaxIndex);
    /**
     * @brief popChannel Entfernt den letzten Channel
     */
    Q_INVOKABLE void popChannel(){removeChannels(getNumberOfChannels()-1);}
    Q_INVOKABLE void pushChannel(QString name, QString description=""){addChannel(getNumberOfChannels(),name,description);}
    /**
     * @brief addChannelDescription Setzt ein Channel neu oder fügt einen neuen Channel mit dem jeweiligen Channel hinzu
     * Wenn eine Lücke zwischen den bisherigen maximalen Index und dem neuen Index liegt, wird diese mit Dummy Channeln aufgefüllt.
     * @param channel Der Index des Channels, darf größer als getNumberOfChannels Sein.
     * @param description die Beschreibung des Channels.
     */
    void addChannel(int channel, QString name, QString description="");

    const Channel * getChannelByName(const QString &name)const;
    const Channel * getChannelById(const ID::value_type id)const;
    const Channel * getChannelByIndex(const unsigned int channelIndex)const;

    const std::vector<std::unique_ptr<Channel>> & getChannels()const{return channels.getVector();}

    void writeJsonObject(QJsonObject &o)const;

    ChannelVector * getChannelModel(){return &channels;}
    const ChannelVector * getChannelModel()const{return &channels;}

signals:
    void numberOfChannelsChanged();
    void channelAdded(Channel*);
    void channelRemoved(Channel*);

};

} // namespace DMX

#endif // DEVICEPROTOTYPE_H
