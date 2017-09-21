#include "device.h"

Device::Device(const QJsonObject &o):NamedObject(o),IDBase<Device>(o),
    prototype(IDBase<DevicePrototype>::getIDBaseObjectByID(o["prototype"].toString().toLong())),
    startDMXChannel(o["startDMXChannel"].toInt()),
    position(o["position"].toObject()["x"].toInt(),o["position"].toObject()["y"].toInt()){

}

void Device::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase<Device>::writeJsonObject(o);
    o.insert("prototype",prototype?QString::number(prototype->getID().value()):0);
    QJsonObject position;
    position.insert("x",this->position.x());
    position.insert("y",this->position.y());
    o.insert("position",position);
    o.insert("startDMXChannel",startDMXChannel);
}
