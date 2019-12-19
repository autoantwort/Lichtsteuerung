#include "systemvolume.h"
#include <QtDebug>

#ifdef Q_OS_MAC
OSStatus SystemVolume::callback(AudioObjectID inObjectID, UInt32 inNumberAddresses, const AudioObjectPropertyAddress *inAddresses, void * /*inClientData*/) {
    for (UInt32 i = 0; i < inNumberAddresses; ++i) {
        const auto &cur = inAddresses[i];
        if (cur.mScope == kAudioDevicePropertyScopeOutput && cur.mSelector == kAudioDevicePropertyVolumeScalar && cur.mElement == 1 /*LEFT_CHANNEL*/) {
            AudioObjectPropertyAddress volumePropertyAddress = {
                kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 /*LEFT_CHANNEL*/
            };

            Float32 volume;
            UInt32 volumedataSize = sizeof(volume);
            auto result = AudioObjectGetPropertyData(inObjectID, &volumePropertyAddress, 0, nullptr, &volumedataSize, &volume);
            if (result == kAudioHardwareNoError) {
                get().volume = static_cast<double>(volume);
                emit get().volumeChanged();
            }
            break;
        }
    }
    return noErr;
}
#endif

#ifdef Q_OS_WIN
HRESULT Callback::OnNotify(AUDIO_VOLUME_NOTIFICATION_DATA *pNotify) {
    SystemVolume::get().setVolume(static_cast<double>(pNotify->fMasterVolume));
    return S_OK;
}

Callback::~Callback() {
    if (counter != 0) {
        qWarning() << "SystemVolume::Callback: Releasing object with ref count > 0";
    }
}

HRESULT Callback::QueryInterface(const IID & /*riid*/, void ** /*ppvObject*/) {
    qWarning() << "SystemVolume::Callback: What do they want from me?";
    return E_NOINTERFACE;
}
#endif

SystemVolume::SystemVolume() {
#ifdef Q_OS_WIN
    HRESULT hr;
    hr = CoInitialize(nullptr);
    if (hr < 0) {
        return;
    }
    // Get enumerator for audio endpoint devices.
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&pEnumerator));
    if (hr < 0) {
        return;
    }
    // Get peak meter for default audio-rendering device.
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (hr < 0) {
        return;
    }
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&endpointVolume));
    if (hr < 0) {
        return;
    }
    hr = endpointVolume->RegisterControlChangeNotify(&callback);
    if (hr < 0) {
        return;
    }
    // query initial volume
    FLOAT v;
    hr = endpointVolume->GetMasterVolumeLevelScalar(&v);
    if (hr < 0) {
        return;
    }
    volume = static_cast<double>(v);
    emit volumeChanged();
#endif
#ifdef Q_OS_MAC
    AudioObjectPropertyAddress getDefaultOutputDevicePropertyAddress = {kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster};

    UInt32 outputDeviceSize = sizeof(defaultOutputDeviceID);
    OSStatus result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &getDefaultOutputDevicePropertyAddress, 0, nullptr, &outputDeviceSize, &defaultOutputDeviceID);

    if (kAudioHardwareNoError != result) {
        qWarning() << "Failed to get the default output device";
    }

    // register a listener so that we get an event when the volume changed
    AudioObjectPropertyAddress volumePropertyAddress = {
        kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 /*LEFT_CHANNEL*/
    };

    result = AudioObjectAddPropertyListener(defaultOutputDeviceID, &volumePropertyAddress, &callback, nullptr);
    if (result != kAudioHardwareNoError) {
        qWarning() << "Registration of Audio Listener Failed";
    }

    // get initial volume
    Float32 volume;
    UInt32 volumedataSize = sizeof(volume);
    result = AudioObjectGetPropertyData(defaultOutputDeviceID, &volumePropertyAddress, 0, nullptr, &volumedataSize, &volume);
    if (result == kAudioHardwareNoError) {
        this->volume = static_cast<double>(volume);
        emit volumeChanged();
    }
#endif
#ifdef Q_OS_LINUX
    snd_mixer_selem_id_alloca(&sid);

    // sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

    if ((snd_mixer_open(&handle, 0)) < 0) {
        qWarning() << "SystemVolume: Failed to open the mixer";
        return;
    }

    if ((snd_mixer_attach(handle, card)) < 0) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to attach the card 'default'";
        return;
    }
    if ((snd_mixer_selem_register(handle, nullptr, nullptr)) < 0) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to register the mixer";
        return;
    }
    if (snd_mixer_load(handle) < 0) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to load the mixer";
        return;
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to find a mixer element";
        return;
    }
    if (snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv) < 0) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to get the mixer playback volume range";
        return;
    }
    snd_mixer_elem_set_callback(elem, snd_mixer_elem_callback);
    startTimer(16);
    long volume;
    if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &volume) < 0) {
        snd_mixer_close(handle);
        handle = nullptr;
        qWarning() << "SystemVolume: Failed to get the volume";
        return;
    }
    this->volume = (volume - minv) / static_cast<double>(maxv - minv);
    emit volumeChanged();

#endif
}

#ifdef Q_OS_LINUX
void SystemVolume::timerEvent(QTimerEvent * /*event*/) {
    if (handle) {
        snd_mixer_handle_events(handle);
    }
}

int SystemVolume::snd_mixer_elem_callback(snd_mixer_elem_t *elem, unsigned int mask) {
    // testet, don't know why value 1, but only mask == 1 works
    if (mask == 1) {
        long volume;
        if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &volume) >= 0) {
            get().volume = (volume - get().minv) / static_cast<double>(get().maxv - get().minv);
            emit get().volumeChanged();
        } else {
            qWarning() << "SystemVolume: Failed to get the volume";
        }
    }
    return 0;
}
#endif

SystemVolume::~SystemVolume() {
#ifdef Q_OS_WIN
    if (pEnumerator) {
        pEnumerator->Release();
    }
    if (pDevice) {
        pDevice->Release();
    }
    if (endpointVolume) {
        endpointVolume->UnregisterControlChangeNotify(&callback);
        endpointVolume->Release();
    }
    if (client) {
        client->Release();
    }
    CoUninitialize();
#endif
#ifdef Q_OS_LINUX
    if (handle) {
        snd_mixer_close(handle);
    }
#endif
}

void SystemVolume::setVolume(double volume) {
    volume = std::clamp(volume, 0., 1.);
    if (volume != this->volume) {
        this->volume = volume;
        emit volumeChanged();
#ifdef Q_OS_WIN
        if (endpointVolume) {
            endpointVolume->SetMasterVolumeLevelScalar(static_cast<float>(volume), nullptr);
        }
#endif
#ifdef Q_OS_MAC
        if (defaultOutputDeviceID != kAudioDeviceUnknown) {

            AudioObjectPropertyAddress volumePropertyAddress = {
                kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 1 /*LEFT_CHANNEL*/
            };
            Float32 volume = static_cast<Float32>(this->volume);
            auto result = AudioObjectSetPropertyData(defaultOutputDeviceID, &volumePropertyAddress, 0, nullptr, sizeof(volume), &volume);
            if (result != kAudioHardwareNoError) {
                qWarning() << "Can not set system volume";
            } else {
                // setting right channel
                volumePropertyAddress.mElement = 2 /*RIGHT_CHANNEL*/;
                result = AudioObjectSetPropertyData(defaultOutputDeviceID, &volumePropertyAddress, 0, nullptr, sizeof(volume), &volume);
                if (result != kAudioHardwareNoError) {
                    qWarning() << "Can not set system volume of right channel";
                }
            }
        }
#endif
#ifdef Q_OS_LINUX
        if (handle) {
            long outvol = static_cast<long>(volume * (maxv - minv)) + minv;
            if (snd_mixer_selem_set_playback_volume_all(elem, outvol) < 0) {
                qWarning() << "SystemVolume: Can not set system volume for all channels";
            }
        }
#endif
    }
}
