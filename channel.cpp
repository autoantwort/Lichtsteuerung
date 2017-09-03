#include "channel.h"

Channel::Channel(const QJsonObject &o):id(o),index(o["index"].toInt()),name(o["name"].toString()),description(o["description"].toString()){}


void Channel::writeJsonObject(QJsonObject &o) const{
    id.writeJsonObject(o);
    o.insert("index",index);
    o.insert("name",name);
    o.insert("description",description);
}
