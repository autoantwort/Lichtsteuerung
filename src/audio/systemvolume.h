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

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#endif

#ifdef Q_OS_WIN
class Callback : public IAudioEndpointVolumeCallback {

    ULONG counter = 0;
    HRESULT OnNotify(AUDIO_VOLUME_NOTIFICATION_DATA *pNotify) override;

public:
    ~Callback();
    HRESULT QueryInterface(REFIID /*riid*/, void ** /*ppvObject*/) override;
    ULONG AddRef() override { return ++counter; }
    ULONG Release() override { return --counter; }
};

#endif

class SystemVolume : public QObject {
    Q_OBJECT
#ifdef Q_OS_WIN
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioEndpointVolume *endpointVolume = nullptr;
    WAVEFORMATEX *wformat = nullptr;
    IAudioClient *client = nullptr;
    Callback callback;
#endif
#ifdef Q_OS_MAC
    AudioDeviceID defaultOutputDeviceID = kAudioDeviceUnknown;
#endif
#ifdef Q_OS_LINUX
    // code from here: https://stackoverflow.com/questions/7657624/get-master-sound-volume-in-c-in-linux
    snd_mixer_t *handle = nullptr;
    snd_mixer_elem_t *elem = nullptr;
    snd_mixer_selem_id_t *sid = nullptr;
    long minv, maxv;

    static constexpr const char *mix_name = "Master";
    static constexpr const char *card = "default";
    static constexpr int mix_index = 0;
#endif
    double volume = -1;
    Q_PROPERTY(double volume READ getVolume WRITE setVolume NOTIFY volumeChanged)

    SystemVolume();

#ifdef Q_OS_LINUX
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    static int snd_mixer_elem_callback(snd_mixer_elem_t *elem, unsigned int mask);
#endif

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
