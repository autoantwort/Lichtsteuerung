#include "mqttimpl.h"
#include "modelmanager.h"

Modules::MqttImpl::MqttImpl() {
    // the objects gets created in the application main thread, but must live in the controller thread.
    // So we have to create the object in the controller Thread, but setLastWillMessage and connect can already be called in
    // the main thread, so we have to delay the execution of these methonds
    ModuleManager::singletone()->controller().runInController([this]() {
        client = std::make_unique<QMqttClient>();
        QObject::connect(client.get(), &QMqttClient::connected, [this] {
            for (const auto &i : queried) {
                if (client->state() != QMqttClient::Connected) {
                    // maybe we lose connection. Then we don't want to crash (subscribe adds than to queried)
                    return;
                }
                subscribe(i.first, i.second);
            }
            queried.clear();
        });
        if (lastWillDelayed) {
            client->setWillTopic(std::get<0>(*lastWillDelayed));
            client->setWillMessage(std::get<1>(*lastWillDelayed));
            client->setWillRetain(std::get<2>(*lastWillDelayed));
        }
        if (connectDelayed) {
            client->setHostname(std::get<0>(*connectDelayed));
            client->setPort(std::get<1>(*connectDelayed));
            client->connectToHost();
        }
    });
}

Modules::MqttImpl::~MqttImpl() {
    ModuleManager::singletone()->controller().runInController([c = client.release()] { delete c; });
}

void Modules::MqttImpl::setLastWillMessage(const std::string &topic, const std::string &message, bool retain) {
    if (client) {
        client->setWillTopic(QString::fromStdString(topic));
        client->setWillMessage(QByteArray::fromStdString(message));
        client->setWillRetain(retain);
    } else {
        lastWillDelayed.emplace(QString::fromStdString(topic), QByteArray::fromStdString(message), retain);
    }
}
void Modules::MqttImpl::connect(const std::string &host, int port) {
    if (client) {
        client->setPort(port);
        client->setHostname(QString::fromStdString(host));
        client->connectToHost();
    } else {
        connectDelayed.emplace(QString::fromStdString(host), port);
    }
}

void Modules::MqttImpl::publishMessage(const std::string &topic, const std::string &message) {
    publish(topic, message, false);
}

void Modules::MqttImpl::publishValue(const std::string &topic, const std::string &message) {
    publish(topic, message, true);
}

void Modules::MqttImpl::subscribe(const std::string &topic, std::function<void(std::string)> callback) {
    if (!client || client->state() != QMqttClient::Connected) {
        // when not yet connected, wait for a connection and add than
        queried.emplace_back(topic, callback);
        return;
    }
    auto funcPointer = callback.target_type().name();
    if (callbacks.find(std::make_pair(topic, funcPointer)) != callbacks.end()) {
        // prevent double registrations of callbacks for the same topic
        return;
    }
    callbacks.emplace(topic, funcPointer);
    auto sub = client->subscribe(QString::fromStdString(topic));
    QObject::connect(sub, &QMqttSubscription::messageReceived, [=](QMqttMessage m) { callback(m.payload().toStdString()); });
}

Modules::MqttClientStatus Modules::MqttImpl::status() {
    switch (client->state()) {
    case QMqttClient::Connected: return MqttClientStatus::Connected;
    case QMqttClient::Connecting: return MqttClientStatus::Connecting;
    case QMqttClient::Disconnected: return MqttClientStatus::Disconnected;
    }
}

void Modules::MqttImpl::publish(const std::string &topic, const std::string &message, bool retain) {
    client->publish(QString::fromStdString(topic), QByteArray::fromStdString(message), 1, retain);
}
