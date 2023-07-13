#include "device.h"
#include <QJsonArray>
#include <QQmlEngine>
#include <modelmanager.h>

namespace DMX {

Device::Device(const QJsonObject &o)
    : NamedObject(o), IDBase<Device>(o), prototype(ModelManager::get().getDevicePrototypeById(o["prototype"])), startDMXChannel(o["startDMXChannel"].toInt()),
      position(o["position"].toObject()["x"].toInt(), o["position"].toObject()["y"].toInt()) {
    if (prototype == nullptr) {
        std::cerr << "Availible Device Prototypes : ";
        for (const auto &r : ModelManager::get().getDevicePrototypes()) {
            std::cerr << r->getID().value() << ' ';
        }
        std::cerr << "Search for Device Prototype with ID : " << o["prototype"].toString().toLongLong();
        throw std::runtime_error("Dont find DevicePrototype with id : " + o["prototype"].toString().toStdString());
    }
    const auto filter = o["filter"].toArray();
    for (const auto f_ : filter) {
        const auto f = f_.toObject();
        const auto id = f["channel"].toString().toLongLong();
        for (auto i = getChannels().cbegin(); i != getChannels().cend(); ++i) {
            if ((**i).getID() == id) {
                this->filter.emplace_back(i->get(), std::make_unique<DMXChannelFilter>(f["filter"].toObject()));
                break;
            }
        }
    }
}

void Device::writeJsonObject(QJsonObject &o) const {
    NamedObject::writeJsonObject(o);
    IDBase<Device>::writeJsonObject(o);
    o.insert("prototype", prototype ? QString::number(prototype->getID().value()) : 0);
    QJsonObject position;
    position.insert("x", this->position.x());
    position.insert("y", this->position.y());
    o.insert("position", position);
    o.insert("startDMXChannel", static_cast<int>(startDMXChannel));
    QJsonArray filter;
    for (const auto &f : this->filter) {
        QJsonObject o;
        o.insert("channel", QString::number(f.first->getID().value()));
        QJsonObject fil;
        f.second->writeJsonData(fil);
        o.insert("filter", fil);
        filter.append(o);
    }
    o.insert("filter", filter);
}

void Device::channelRemoved(Channel *c) {
    for (auto i = filter.cbegin(); i != filter.cend(); ++i) {
        if (i->first == c) {
            filter.erase(i);
            return;
        }
    }
}

void Device::channelAdded(Channel *c) {
    filter.emplace_back(c, std::make_unique<DMXChannelFilter>());
}

DMXChannelFilter *Device::getFilterForChannel(Channel *c) {
    for (auto i = filter.cbegin(); i != filter.cend(); ++i) {
        if (i->first == c) {
            QQmlEngine::setObjectOwnership(i->second.get(), QQmlEngine::CppOwnership);
            return i->second.get();
        }
    }
    filter.emplace_back(c, std::make_unique<DMXChannelFilter>());
    QQmlEngine::setObjectOwnership(filter.back().second.get(), QQmlEngine::CppOwnership);
    return filter.back().second.get();
}

DMXChannelFilter *Device::getFilterForChannelindex(int index) {
    for (auto i = getChannels().cbegin(); i != getChannels().cend(); ++i) {
        if ((**i).getIndex() == index) {
            return getFilterForChannel(i->get());
        }
    }
    return nullptr;
}

} // namespace DMX
