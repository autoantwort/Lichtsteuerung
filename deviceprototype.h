#ifndef DEVICEPROTOTYPE_H
#define DEVICEPROTOTYPE_H

#include <QString>
#include <vector>
#include <QVector>
#include "channel.h"
#include "idbase.h"

/** Jedes Gerät(Scanner/Laser/Lampe/...) bekommt ein Device Prototype, wo festgelegt wird, auf welchem Channel welche Daten anliegen
 * @brief The DevicePrototype class
 */
class DevicePrototype : public NamedObject, public IDBase<DevicePrototype>
{
    Q_OBJECT
    Q_PROPERTY(int numberOfChannels READ getNumberOfChannels NOTIFY numberOfChannelsChanged)
private:
    /**
     * @brief channels Ein vector die die einzelnen Channel Informationen hält
     */
    std::vector<Channel*> channels;
public:
    static QString syncServiceClassName;
    DevicePrototype(const QJsonObject &o);
    DevicePrototype(QString name):NamedObject(name,&syncServiceClassName){}
    int getNumberOfChannels()const{return channels.size();}
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
    const Channel * getChannelByIndex(const unsigned int channelIndex)const;

    const std::vector<Channel*> & getChannels()const{return channels;};

    void writeJsonObject(QJsonObject &o)const;
public:
    static void update (const ID &id, const QString &name,const QString &value){
        auto d = IDBase<DevicePrototype>::getIDBaseObjectByID(id);
        if(d){
            auto s = name.toLatin1();
            d->setProperty(s.data(),QVariant(value));
        }
    }
    static void create (const QJsonObject &o){new DevicePrototype(o);}
    static void remove (const ID &id){
        auto d = IDBase<DevicePrototype>::getIDBaseObjectByID(id);
        if(d){
            delete d;
        }
    }
    static void createMember (const ID &id,const QString &name,const QJsonObject &o){
        auto d = IDBase<DevicePrototype>::getIDBaseObjectByID(id);
        if(d&&name=="channels")d->addChannel(o["index"].toInt(),o["name"].toString(),o["description"].toString());
    }
    static void removeMember (const ID &pid,const QString &name,const ID &id){
        auto d = IDBase<DevicePrototype>::getIDBaseObjectByID(pid);
        if(d&&name=="channels"){
            auto c = IDBase<Channel>::getIDBaseObjectByID(id);
            if(c){
                d->removeChannels(c->getIndex());
            }
        }
    }

signals:
    void numberOfChannelsChanged();
    void channelAdded(Channel*);
    void channelRemoved(Channel*);

};

#endif // DEVICEPROTOTYPE_H
