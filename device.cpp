#include "device.h"

QString Device::syncServiceClassName;

Device::Device(const QJsonObject &o):NamedObject(o,&syncServiceClassName),IDBase<Device>(o),
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
    o.insert("startDMXChannel",static_cast<int>(startDMXChannel));
}

void Device::channelRemoved(Channel *c){
    for(auto i = filter.cbegin();i!=filter.cend();++i){
        if(i->first==c){
            delete i->second;
            filter.erase(i);
            return;
        }
    }
}

void Device::channelAdded(Channel *c){
    filter.emplace_back(c,new DMXChannelFilter());
}
