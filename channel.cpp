#include "channel.h"

Channel::Channel(const QJsonObject &o):NamedObject(o,&syncServiceClassName),IDBase<Channel>(o),index(o["index"].toInt()){}


void Channel::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase<Channel>::writeJsonObject(o);
    o.insert("index",index);
}
