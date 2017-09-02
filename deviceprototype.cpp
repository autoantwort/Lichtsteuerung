#include "deviceprototype.h"
#include <QJsonArray>

void DevicePrototype::removeChannels(int newMaxIndex){
    if (newMaxIndex<0||newMaxIndex>getNumberOfChannels()) {
        return;
    }
    // Elemente ganz hinten löschen
    for (int i = 0; i < getNumberOfChannels() - newMaxIndex; ++i) {
        channels.pop_back();
    }
}

void DevicePrototype::addChannel(int channel, QString name, QString description){
    // dummy Channels einfügen
    if (channel>=getNumberOfChannels()) {
        for (int i = getNumberOfChannels(); i <= channel; ++i) {
            channels.push_back(Channel(i));
        }
    }
    // eigenschaften setzten
    channels[channel].name = name;
    channels[channel].description = description;
}

const Channel * DevicePrototype::getChannelById(const int id) const{
    for(const auto channel : channels){
        if (channel == id) {
            return &channel;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByName(const QString &name) const{
    for(const auto channel : channels){
        if (channel.name == name) {
            return &channel;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByIndex(const int channelIndex) const{
    if (channelIndex<0||channelIndex>=channels.size()) {
        return nullptr;
    }
    return &channels[channelIndex];
}

DevicePrototype::DevicePrototype(const QJsonObject &o):id(o){
    auto array = o["channels"].toArray();
    for(const auto c : array){
        channels.push_back(Channel(c.toObject()));
    }
}

void DevicePrototype::writeJsonObject(QJsonObject &o) const{
    id.writeJsonObject(o);
    o.insert("name",name);
    QJsonArray channels;
    for(const auto c : this->channels){
        QJsonObject o;
        c.writeJsonObject(o);
        channels.append(o);
    }
    o.insert("channels",channels);
}
