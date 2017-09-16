#include "device.h"

Device::Device(const QJsonObject &o):IDBase(o),
    prototype(IDBase<DevicePrototype>::getIDBaseObjectByID(o["prototype"].toString().toLong())),
    name(o["name"].toString()),
    desciption(o["description"].toString()),
    position(o["position"].toObject()["x"].toInt(),o["position"].toObject()["y"].toInt()),
    startDMXChannel(o["startDMXChannel"].toInt()){

}

void Device::writeJsonObject(QJsonObject &o) const{
    IDBase::writeJsonObject(o);
    o.insert("prototype",prototype?QString::number(prototype->getID().value()):0);
    o.insert("name",name);
    o.insert("desciption",desciption);
    QJsonObject position;
    position.insert("x",this->position.x());
    position.insert("y",this->position.y());
    o.insert("position",position);
    o.insert("startDMXChannel",startDMXChannel);
}
