#include "deviceprototype.h"
#include <QJsonArray>

namespace DMX{

void ChannelVector::beginPushBack(int length){
    beginInsertRows(QModelIndex(),channels.size(),channels.size()+length);
}

void ChannelVector::endPushBack(){
    endInsertRows();
}

void ChannelVector::pop_back(){
    const auto pos = channels.size()-1;
    beginRemoveRows(QModelIndex(),pos,pos);
    channels.pop_back();
    endRemoveRows();
}

void DevicePrototype::removeChannels(int newMaxIndex){
    if (newMaxIndex<0||newMaxIndex>getNumberOfChannels()) {
        return;
    }
    // Elemente ganz hinten löschen
    for (int i = 0; i < getNumberOfChannels() - newMaxIndex; ++i) {
        channelRemoved(channels.getChannel().back());
        delete channels.getChannel().back();
        channels.pop_back();
    }
}


void DevicePrototype::addChannel(int channel, QString name, QString description){
    // dummy Channels einfügen
    if (channel>=getNumberOfChannels()) {
        channels.beginPushBack(getNumberOfChannels()-channel);
        for (int i = getNumberOfChannels(); i < channel; ++i) {
            channels.getChannel().push_back(new Channel(i));
            emit channelAdded(channels.getChannel().back());
            QJsonObject o;
            channels.getChannel().back()->writeJsonObject(o);
        }        
        channels.getChannel().push_back(new Channel(channel,name,description));
        channels.endPushBack();
        emit channelAdded(channels.getChannel().back());
        QJsonObject o;
        channels.getChannel().back()->writeJsonObject(o);
    }else{
        // eigenschaften setzten
        channels.getChannel()[channel]->setName(name);
        channels.getChannel()[channel]->setDescription(description);
    }
}

const Channel * DevicePrototype::getChannelById(const int id) const{
    for(auto c = channels.getChannel().cbegin();c!=channels.getChannel().cend();++c){
        if ((**c).getID().value() == id) {
            return *c;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByName(const QString &name) const{
    for(auto c = channels.getChannel().cbegin();c!=channels.getChannel().cend();++c){
        if ((**c).getName() == name) {
            return *c;
        }
    }
    return nullptr;
}

const Channel * DevicePrototype::getChannelByIndex(const unsigned int channelIndex) const{
    if (channelIndex>=channels.getChannel().size()) {
        return nullptr;
    }
    return channels.getChannel()[channelIndex];
}

DevicePrototype::DevicePrototype(const QJsonObject &o):NamedObject(o),IDBase(o){
    auto array = o["channels"].toArray();
    for(const auto c : array){
        channels.getChannel().push_back(new Channel(c.toObject()));
    }
}

void DevicePrototype::writeJsonObject(QJsonObject &o) const{
    NamedObject::writeJsonObject(o);
    IDBase::writeJsonObject(o);
    QJsonArray channels;
    for(const auto c : this->channels.getChannel()){
        QJsonObject o;
        c->writeJsonObject(o);
        channels.append(o);
    }
    o.insert("channels",channels);
}

} // namespace DMX
