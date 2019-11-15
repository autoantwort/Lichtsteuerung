#ifndef REMOTEVOLUME_H
#define REMOTEVOLUME_H

#include <QObject>
#include <QWebSocket>
#include <settings.h>

class RemoteVolume : public QObject {
    Q_OBJECT
    QWebSocket webSocket;
    Settings &settings;
    int reconnectTimerId = -1;
    int pingTimer = -1;
    static constexpr int WAIT_FOR_RECONNECT_MS = 5 * 1000;
    static constexpr int PING_INTERVALL_MS = 59 * 1000;
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
public:
    explicit RemoteVolume(Settings &settings);
    void connect();
    [[nodiscard]] bool isConnected() const { return webSocket.isValid(); }

protected:
    void timerEvent(QTimerEvent *event) override;
signals:
    void isConnectedChanged();
};

#endif // REMOTEVOLUME_H
