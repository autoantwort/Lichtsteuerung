#ifndef DEVICEPROTOTYPE_H
#define DEVICEPROTOTYPE_H

#include <QString>
#include <vector>
#include <QVector>
#include "channel.h"

/** Jedes Gerät(Scanner/Laser/Lampe/...) bekommt ein Device Prototype, wo festgelegt wird, auf welchem Channel welche Daten anliegen
 * @brief The DevicePrototype class
 */
class DevicePrototype
{
private:
    ID id;
    /**
     * @brief name Der Name des DevicePrototypen, zb Scanner, Licht, Laser, ...
     */
    QString name;
    /**
     * @brief channels Ein vector die die einzelnen Channel Informationen hält
     */
    std::vector<Channel> channels;
public:
    DevicePrototype(const QJsonObject &o);
    DevicePrototype(QString name):name(name){}
    int getNumberOfChannels()const{return channels.size()+1;}
    /**
     * @brief removeChannels Entfernt Channel bis zu einem bestimmten Index
     * @param newMaxIndex Der neue Maximale Index (Inclusiv)
     */
    void removeChannels(int newMaxIndex);
    /**
     * @brief addChannelDescription Setzt ein Channel neu oder fügt einen neuen Channel mit dem jeweiligen Channel hinzu
     * Wenn eine Lücke zwischen den bisherigen maximalen Index und dem neuen Index liegt, wird diese mit Dummy Channeln aufgefüllt.
     * @param channel Der Index des Channels, darf größer als getNumberOfChannels Sein.
     * @param description die Beschreibung des Channels.
     */
    void addChannel(int channel, QString name, QString description="");

    const Channel * getChannelByName(const QString &name)const;
    const Channel * getChannelById(const int id)const;
    const Channel * getChannelByIndex(const int channelIndex)const;

    void writeJsonObject(QJsonObject &o)const;

    const ID& getID()const{return id;}
};

#endif // DEVICEPROTOTYPE_H
