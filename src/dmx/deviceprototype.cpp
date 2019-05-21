#include "deviceprototype.h"
#include <QJsonArray>

namespace DMX{


void DevicePrototype::removeChannels(int newMaxIndex){
    if (newMaxIndex<0||newMaxIndex>getNumberOfChannels()) {
        return;
    }
    // Elemente ganz hinten löschen
    for (int i = 0; i < getNumberOfChannels() - newMaxIndex; ++i) {
        channelRemoved(channels.getVector().back().get());
        channels.erase(std::next(channels.begin(),static_cast<int>(channels.size())-1));
    }
}


void DevicePrototype::addChannel(int channel, QString name, QString description){
    // dummy Channels einfügen
    if (channel>=getNumberOfChannels()) {
        channels.beginPushBack(getNumberOfChannels()-channel);
        for (int i = getNumberOfChannels(); i < channel; ++i) {
            channels.push_back(std::make_unique<Channel>(i));
            emit channelAdded(channels.getVector().back().get());
            QJsonObject o;
            channels.back()->writeJsonObject(o);
        }        
        channels.getVector().push_back(std::make_unique<Channel>(channel,name,description));
        channels.endPushBack();
        emit channelAdded(channels.getVector().back().get());
        QJsonObject o;
        channels.getVector().back()->writeJsonObject(o);
    }else{
        // eigenschaften setzten
        channels.getVector()[channel]->setName(name);
        channels.getVector()[channel]->setDescription(description);
    }
}

const Channel * DevicePrototype::getChannelById(const ID::value_type id) const{
    for(auto c = channels.getVector().cbegin();c!=channels.getVector().cend();++c){
        if ((**c).getID().value() == id) {
            return c->get();
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByName(const QString &name) const{
    for(auto c = channels.getVector().cbegin();c!=channels.getVector().cend();++c){
        if ((**c).getName() == name) {
            return c->get();
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByIndex(const unsigned int channelIndex) const{
    if (channelIndex>=channels.getVector().size()) {
        return nullptr;
    }
    return channels.getVector()[channelIndex].get();
}

DevicePrototype::DevicePrototype(const QJsonObject &o):NamedObject(o),IDBase(o){
    auto array = o["channels"].toArray();
    for(const auto c : array){
        channels.push_back(std::make_unique<Channel>(c.toObject()));
    }
}

void DevicePrototype::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase::writeJsonObject(o);
    QJsonArray channels;
    for(const auto & c : this->channels.getVector()){
        QJsonObject o;
        c->writeJsonObject(o);
        channels.append(o);
    }
    o.insert("channels",channels);
}

} // namespace DMX
