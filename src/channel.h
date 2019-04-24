#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "idbase.h"
#include "namedobject.h"

/**
 * @brief The Channel class give us information about a channel in a devicePrototype
 * Eine Klasse die die jeweiligen ChannelInformationen für ein deviecPrototype hält
 */
class Channel : public NamedObject, public IDBase<Channel>
{
    Q_OBJECT
    Q_PROPERTY(int index READ getIndex)
private:
    /**
     * @brief index Der DMX Index (Offset) von der späteren tatsächlichen start Adresse eines bestimmten Geräts
     */
    int index;
    friend class DevicePrototype;
public:
    Channel(int index, QString name = "Unknown", QString description=""):NamedObject(name,description),index(index){}
    Channel(const QJsonObject &o);

    int getIndex()const{return index;}

    void writeJsonObject(QJsonObject &o)const;

};

#endif // CHANNEL_H
