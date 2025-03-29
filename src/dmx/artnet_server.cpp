#include "artnet_server.h"

#include "errornotifier.h"
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <QtEndian>

using namespace std::chrono;

ArtNetReceiver::ArtNetReceiver(QObject *parent) : QObject(parent) {
    socket = new QUdpSocket(this);
    if (!socket->bind(QHostAddress::Any, 6454)) { // ArtNet typically uses port 6454
        qDebug() << "Failed to bind to port 6454";
        ErrorNotifier::showError(QStringLiteral("Port 6454 ist bereits benutzt (vielleicht QLC). Kann keine ArtNet Pakete empfangen."));
        return;
    }

    connect(socket, &QUdpSocket::readyRead, this, &ArtNetReceiver::processPendingDatagrams);
}

void ArtNetReceiver::writeDefaultValuesTo(span<unsigned char> output_data) const {
    if (system_clock::now() - last_default_values > DURATION_IGNORE_ARTNET) {
        return;
    }
    std::copy(default_values, default_values + std::min(output_data.size(), MAX_CHANNEL), output_data.data());
}

void ArtNetReceiver::writeKeepValues(span<unsigned char> output_data) const {
    for (auto &iter : keep_channels) {
        if (system_clock::now() - iter.second.last_keep_values > DURATION_IGNORE_ARTNET) {
            continue;
        }
        for (size_t i = 0; i < std::min(output_data.size(), MAX_CHANNEL); i++) {
            if (iter.second.keep_channels[i]) {
                output_data[i] = static_cast<unsigned char>(iter.second.keep_values[i]);
            }
        }
    }
}

void ArtNetReceiver::writeOverrideValues(span<unsigned char> output_data) const {
    if (system_clock::now() - last_override_values > DURATION_IGNORE_ARTNET) {
        return;
    }
    std::copy(override_values, override_values + std::min(output_data.size(), MAX_CHANNEL), output_data.data());
}

void ArtNetReceiver::processPendingDatagrams() {
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QByteArray data = datagram.data();

        // Check if it's an ArtNet packet
        if (data.startsWith("Art-Net")) {
            // Check if it's an ArtDmx packet (OpCode 0x5000)
            quint16 opCode = (quint8)data[8] | ((quint16)(quint8)data[9] << 8);
            if (opCode == 0x5000) {
                // qDebug() << "Received ArtDmx packet:";
                // qDebug() << "Sequence:" << (quint8)data[12];
                // qDebug() << "Physical:" << (quint8)data[13];
                quint16 universe = qFromLittleEndian<quint16>(reinterpret_cast<const uchar *>(data.constData() + 14));
                // qDebug() << "Universe:" << universe;
                quint16 length = qFromBigEndian<quint16>(reinterpret_cast<const uchar *>(data.constData() + 16));
                // qDebug() << "Data length:" << length;
                auto values = data.mid(18, std::min<size_t>(length, MAX_CHANNEL));
                // qDebug() << "DMX Data:" << values.toHex();
                if (universe < 100) {
                    std::copy(values.begin(), values.end(), default_values);
                    last_default_values = system_clock::now();
                } else if (universe < 200) {
                    auto &entry = keep_channels[universe];
                    for (size_t i = 0; i < values.size(); i++) {
                        if (values[i] > 0) {
                            entry.keep_channels[i] = true;
                        }
                    }
                    std::copy(values.begin(), values.end(), entry.keep_values);
                    entry.last_keep_values = system_clock::now();
                } else {
                    std::copy(values.begin(), values.end(), override_values);
                    last_override_values = system_clock::now();
                }
            }
        }
    }
}
