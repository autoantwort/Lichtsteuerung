#include "systemvolume.h"

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
#endif
    startTimer(SystemVolumeUpdateRateInMs);
    timerEvent(nullptr);
}

void SystemVolume::timerEvent(QTimerEvent * /*event*/) {
#ifdef Q_OS_WIN
    if (endpointVolume) {
        float vol;
        auto hr = endpointVolume->GetMasterVolumeLevelScalar(&vol);
        if (hr >= 0 /* success */) {
            auto v = static_cast<double>(vol);
            if (volume != v) {
                volume = v;
                emit volumeChanged();
            }
        }
    }
#endif
}

SystemVolume::~SystemVolume() {
    if (pEnumerator) {
        pEnumerator->Release();
    }
    if (pDevice) {
        pDevice->Release();
    }
    if (endpointVolume) {
        endpointVolume->Release();
    }
    if (client) {
        client->Release();
    }
    CoUninitialize();
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
    }
}
