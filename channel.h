#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "idbase.h"

/**
 * @brief The Channel class give us information about a channel in a devicePrototype
 * Eine Klasse die die jeweiligen ChannelInformationen für ein deviecPrototype hält
 */
class Channel : public IDBase<Channel>
{
private:
    /**
     * @brief index Der DMX Index (Offset) von der späteren tatsächlichen start Adresse eines bestimmten Geräts
     */
    int index;
    /**
     * @brief name Der Name des Channels, zb Red, Green, Blue, Speed, Programm, ...
     */
    QString name;
    /**
     * @brief description Eine Beschreibung des Channels, zb um deutlich zu machen, was er macht
     */
    QString description;

    friend class DevicePrototype;
public:
    Channel(int index, QString name = "Unknown", QString description=""):index(index),name(name),description(description){}
    Channel(const QJsonObject &o);

    int getIndex()const{return index;}
    QString getName()const{return name;}
    void setName(const QString &newName){name = newName;}
    void setDescription(const QString &newDescription){name = newDescription;}
    QString getDescription()const{return description;}


    void writeJsonObject(QJsonObject &o)const;
};

#endif // CHANNEL_H
