#include "remotevolume.h"
#include "systemvolume.h"

RemoteVolume::RemoteVolume(Settings &settings) : settings(settings) {
    QObject::connect(&settings, &Settings::computerNameChanged, [this]() {
        if (isConnected()) {
            webSocket.sendTextMessage("Name:" + this->settings.getComputerName());
        }
    });
    QObject::connect(&settings, &Settings::remoteVolumeControlChanged, [this]() {
        if (this->settings.remoteVolumeControl()) {
            if (!isConnected()) {
                connect();
            }
        } else {
            webSocket.close();
        }
    });

    QObject::connect(&webSocket, &QWebSocket::connected, [this]() {
        webSocket.sendTextMessage("Name:" + this->settings.getComputerName());
        webSocket.sendTextMessage("Value:" + QString::number(SystemVolume::get().getVolume()));
    });
    QObject::connect(&webSocket, &QWebSocket::textMessageReceived, [](const QString &message) {
        bool ok;
        double vol = message.toDouble(&ok);
        if (ok) {
            SystemVolume::get().setVolume(vol);
        }
    });
    QObject::connect(&SystemVolume::get(), &SystemVolume::volumeChanged, [this]() {
        if (webSocket.isValid()) {
            webSocket.sendTextMessage("Value:" + QString::number(SystemVolume::get().getVolume()));
        }
    });
    if (settings.remoteVolumeControl()) {
        connect();
    }
}

void RemoteVolume::connect() {
    webSocket.open(QUrl(QStringLiteral("wss://orga.symposion.hilton.rwth-aachen.de/volumeClient")));
}
