#ifndef SYSTEMVOLUME_H
#define SYSTEMVOLUME_H

#include <QObject>

#ifdef Q_OS_WIN
// windows code from here: http://www.rohitab.com/discuss/topic/43988-increasedecreasemute-the-master-volume-in-windows/
#include <audioclient.h>
#include <conio.h>
#include <endpointvolume.h>
#include <gdiplus.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include <CoreAudio/CoreAudio.h>
#endif

class SystemVolume : public QObject {
    Q_OBJECT
#ifdef Q_OS_WIN
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioEndpointVolume *endpointVolume = nullptr;
    WAVEFORMATEX *wformat = nullptr;
    IAudioClient *client = nullptr;
#endif
#ifdef Q_OS_MAC
    AudioDeviceID defaultOutputDeviceID = kAudioDeviceUnknown;
#endif
    double volume = -1;
    Q_PROPERTY(double volume READ getVolume WRITE setVolume NOTIFY volumeChanged)

    SystemVolume();
    static constexpr int SystemVolumeUpdateRateInMs = 10'000;

protected:
    void timerEvent(QTimerEvent *event) override;

public:
    static SystemVolume &get() {
        static SystemVolume sv;
        return sv;
    }
    ~SystemVolume() override;
    [[nodiscard]] double getVolume() const { return volume; }
    void setVolume(double volume);
signals:
    void volumeChanged();
};

#endif // SYSTEMVOLUME_H
