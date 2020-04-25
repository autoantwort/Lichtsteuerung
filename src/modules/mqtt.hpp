#ifndef MQTT_HPP
#define MQTT_HPP

#include <functional>
#include <memory>
#include <string>

namespace Modules {

enum class MqttClientStatus { Disconnected, Connecting, Connected, NotAvailible };
namespace detail {

    class IMqttClientImpl {
    public:
        virtual void setLastWillMessage(const std::string &topic, const std::string &message, bool retain) = 0;
        virtual void connect(const std::string &host, int port) = 0;
        virtual void publishMessage(const std::string &topic, const std::string &message) = 0;
        virtual void publishValue(const std::string &topic, const std::string &message) = 0;
        virtual void subscribe(const std::string &topic, std::function<void(std::string)> callback) = 0;
        virtual MqttClientStatus status() = 0;
        virtual ~IMqttClientImpl() {}
    };

    template <typename Type>
    bool convert(const std::string &s, Type &value) noexcept {
        static_assert(std::is_integral_v<Type>, "The value must be an integral value like int, float, long, double, ...");
        using T = std::make_signed_t<Type>;
        try {
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool> || std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t>)
                value = std::stoi(s);
            else if constexpr (std::is_same_v<T, long>)
                value = std::stol(s);
            else if constexpr (std::is_same_v<T, long long>)
                value = std::stoll(s);
            else if constexpr (std::is_same_v<T, float>)
                value = std::stof(s);
            else if constexpr (std::is_same_v<T, double>)
                value = std::stod(s);
            else if constexpr (std::is_same_v<T, long double>)
                value = std::stod(s);
        } catch (std::logic_error &) {
            return false;
        }
    }
} // namespace detail
} // namespace Modules
extern "C" {
Modules::detail::IMqttClientImpl *_createMqttClientImpl();
}
namespace Modules {

class MqttClient {
    std::unique_ptr<Modules::detail::IMqttClientImpl> impl;

public:
    MqttClient() : impl(_createMqttClientImpl()) {
        if (!impl) throw std::runtime_error("MQTT Client is nullptr");
    }
    MqttClient(const std::string &lastWillTopic, const std::string &lastWillMessage, bool retain) : MqttClient() { impl->setLastWillMessage(lastWillTopic, lastWillMessage, retain); }
    template <typename T>
    MqttClient(const std::string &lastWillTopic, const T &lastWillValue, bool retain) : MqttClient(lastWillTopic, std::to_string(lastWillValue), retain) {}
    MqttClient(const std::string &host, int port = 1883) : MqttClient() { impl->connect(host, port); }

    void connect(const std::string &host, int port = 1883) { impl->connect(host, port); }
    void publishMessage(const std::string &topic, const std::string &message) { impl->publishMessage(topic, message); }
    template <typename T>
    void publishMessage(const std::string &topic, const T &message) {
        static_assert(std::is_arithmetic_v<T>, "T must be a arithmetic type");
        impl->publishMessage(topic, std::to_string(message));
    }
    void publishValue(const std::string &topic, const std::string &message) { impl->publishValue(topic, message); }
    template <typename T>
    void publishValue(const std::string &topic, const T &message) {
        static_assert(std::is_arithmetic_v<T>, "T must be a arithmetic type");
        impl->publishValue(topic, std::to_string(message));
    }
    void subscribe(const std::string &topic, std::function<void(std::string)> callback) { impl->subscribe(topic, callback); }
    template <typename T>
    void subscribe(const std::string &topic, std::function<void(T)> callback) {
        impl->subscribe(topic, [=](std::string s) {
            T value;
            if (detail::convert(s, value)) callback(value);
        });
    }
    template <typename T>
    void subscribe(const std::string &topic, std::function<void(std::string, bool, T)> callback) {
        impl->subscribe(topic, [=](std::string s) {
            T value{};
            bool success = detail::convert(s, value);
            callback(s, success, value);
        });
    }
    MqttClientStatus status() { return impl->status(); }
};

using Mqtt = MqttClient;

} // namespace Modules

#endif // MQTT_HPP
