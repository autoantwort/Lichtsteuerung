#include "systemvolume.h"
#include <QtDebug>

#ifdef Q_OS_MAC
OSStatus callback(AudioObjectID inObjectID, UInt32 inNumberAddresses, const AudioObjectPropertyAddress *inAddresses, void * /*inClientData*/) {
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
                SystemVolume::get().setVolume(static_cast<double>(volume));
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
}

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
    }
}
