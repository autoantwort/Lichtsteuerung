#ifndef REMOTEVOLUME_H
#define REMOTEVOLUME_H

#include <QWebSocket>
#include <settings.h>

class RemoteVolume
{
    QWebSocket webSocket;
    Settings &settings;

public:
    RemoteVolume(Settings &settings);
    void connect();
    [[nodiscard]] bool isConnected() const { return webSocket.isValid(); }
};

#endif // REMOTEVOLUME_H
