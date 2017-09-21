#include "deviceprototype.h"
#include <QJsonArray>

void DevicePrototype::removeChannels(int newMaxIndex){
    if (newMaxIndex<0||newMaxIndex>getNumberOfChannels()) {
        return;
    }
    // Elemente ganz hinten löschen
    for (int i = 0; i < getNumberOfChannels() - newMaxIndex; ++i) {
        delete channels.back();
        channels.pop_back();
    }
}

void DevicePrototype::addChannel(int channel, QString name, QString description){
    // dummy Channels einfügen
    if (channel>=getNumberOfChannels()) {
        for (int i = getNumberOfChannels(); i <= channel; ++i) {
            channels.push_back(new Channel(i));
        }
    }
    // eigenschaften setzten
    channels[channel]->setName(name);
    channels[channel]->setDescription(description);
}

const Channel * DevicePrototype::getChannelById(const int id) const{
    for(auto c = channels.cbegin();c!=channels.cend();++c){
        if ((**c).getID().value() == id) {
            return *c;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByName(const QString &name) const{
    for(auto c = channels.cbegin();c!=channels.cend();++c){
        if ((**c).getName() == name) {
            return *c;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByIndex(const unsigned int channelIndex) const{
    if (channelIndex>=channels.size()) {
        return nullptr;
    }
    return channels[channelIndex];
}

DevicePrototype::DevicePrototype(const QJsonObject &o):NamedObject(o),IDBase(o){
    auto array = o["channels"].toArray();
    for(const auto c : array){
        channels.push_back(new Channel(c.toObject()));
    }
}

void DevicePrototype::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase::writeJsonObject(o);
    QJsonArray channels;
    for(const auto c : this->channels){
        QJsonObject o;
        c->writeJsonObject(o);
        channels.append(o);
    }
    o.insert("channels",channels);
}
