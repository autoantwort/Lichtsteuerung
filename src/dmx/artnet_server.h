#ifndef ARTNET_SERVER_H
#define ARTNET_SERVER_H

#include "span.h"
#include <QObject>
#include <bitset>
#include <chrono>
#include <map>

class QUdpSocket;

constexpr size_t MAX_CHANNEL = 512;
/**
 * @brief DURATION_IGNORE_ARTNET The Artnet values are ignored if they are older then specified in this constant
 */
constexpr auto DURATION_IGNORE_ARTNET = std::chrono::seconds(20);

class ArtNetReceiver : public QObject {
    Q_OBJECT

public:
    ArtNetReceiver(QObject *parent = nullptr);

    void writeDefaultValuesTo(span<unsigned char> output_data) const;
    void writeKeepValues(span<unsigned char> output_data) const;
    void writeOverrideValues(span<unsigned char> output_data) const;

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *socket;

    unsigned char default_values[MAX_CHANNEL]{};
    std::chrono::system_clock::time_point last_default_values{};

    struct Entry {
        std::bitset<MAX_CHANNEL> keep_channels{};
        unsigned char keep_values[MAX_CHANNEL]{};
        std::chrono::system_clock::time_point last_keep_values{};
    };
    std::map<int, Entry> keep_channels;

    unsigned char override_values[MAX_CHANNEL]{};
    std::chrono::system_clock::time_point last_override_values{};
};

#endif // ARTNET_SERVER_H
