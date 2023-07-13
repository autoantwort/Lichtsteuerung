#include "remotevolume.h"
#include "systemvolume.h"
#include <QTimerEvent>
#include <QtDebug>

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
            // if we were in reconnection mode, stop the timer
            if (reconnectTimerId >= 0) {
                killTimer(reconnectTimerId);
                reconnectTimerId = -1;
            }
        }
    });

    QObject::connect(&webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error),
                     [this](const auto error) { qWarning() << "Remote Volume: WebSocket connection error: " << error << " : " << webSocket.errorString(); });
    QObject::connect(&webSocket, &QWebSocket::disconnected, [this]() {
        emit isConnectedChanged();
        // if we lost the connection, but don't want that, wait some time and reconnect then
        if (this->settings.remoteVolumeControl()) {
            if (reconnectTimerId == -1) {
                reconnectTimerId = startTimer(WAIT_FOR_RECONNECT_MS);
            }
        }
        killTimer(pingTimer);
    });
    QObject::connect(&webSocket, &QWebSocket::connected, [this]() {
        webSocket.sendTextMessage("Name:" + this->settings.getComputerName());
        webSocket.sendTextMessage("Value:" + QString::number(SystemVolume::get().getVolume()));
        emit isConnectedChanged();
        // if we were in reconnection mode, stop the timer
        if (reconnectTimerId >= 0) {
            killTimer(reconnectTimerId);
            reconnectTimerId = -1;
        }
        pingTimer = startTimer(PING_INTERVALL_MS);
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

void RemoteVolume::timerEvent(QTimerEvent *event) {
    if (event->timerId() == reconnectTimerId) {
        event->accept();
        connect();
    } else if (event->timerId() == pingTimer) {
        event->accept();
        webSocket.ping();
    }
}
