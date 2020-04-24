#ifndef MQTTIMPL_H
#define MQTTIMPL_H

#include "mqtt.hpp"
#include <QtMqtt/QMqttClient>
#include <atomic>
#include <memory>
#include <optional>
#include <set>
#include <tuple>

namespace Modules {

class MqttImpl : public detail::IMqttClientImpl {
    std::unique_ptr<QMqttClient> client;
    std::optional<std::tuple<QString, QByteArray, bool>> lastWillDelayed;
    std::optional<std::tuple<QString, int>> connectDelayed;
    std::set<std::pair<std::string, const char *>> callbacks;
    std::vector<std::pair<const std::string &, std::function<void(std::string)>>> queried;
    int test = -1;

public:
    MqttImpl();
    ~MqttImpl() override;
    void setLastWillMessage(const std::string &topic, const std::string &message, bool retain) override;
    void connect(const std::string &host, int port) override;
    void publishMessage(const std::string &topic, const std::string &message) override;
    void publishValue(const std::string &topic, const std::string &message) override;
    void subscribe(const std::string &topic, std::function<void(std::string)> callback) override;
    MqttClientStatus status() override;

protected:
    void publish(const std::string &topic, const std::string &message, bool retain);
};

} // namespace Modules

#endif // MQTTIMPL_H
