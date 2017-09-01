#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "id.h"
class Channel
{
private:
    ID id;
    int index;
    QString name;
    QString description;

    friend class DevicePrototype;
public:
    Channel(int index, QString name = "Unknown", QString description=""):Channel(ID::generateNew(),index,name,description){}
    explicit Channel(ID id, int index, QString name = "Unknown", QString description=""):id(id),index(index),name(name),description(description){}

    const ID getID()const{return id;}
    int getIndex()const{return index;}
    QString getName()const{return name;}
    void setName(const QString &newName){name = newName;}
    void setDescription(const QString &newDescription){name = newDescription;}
    QString getDescription()const{return description;}

    bool operator ==(const ID id)const{return this->id==id;}
    bool operator ==(const int id)const{return this->id.value()==id;}
};

#endif // CHANNEL_H
