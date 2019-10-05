#include "audiocapturemanager.h"
#include "errornotifier.h"
#include "gui/colorplot.h"
#include "gui/graph.h"
#include "gui/oscillogram.h"
#include <algorithm>

namespace Audio {
AudioCaptureManager::AudioCaptureManager():audiofft(sample.size())
{
    rtAudio.showWarnings();
    updateCaptureDeviceList();
}

int AudioCaptureManager::rtAudioCallback(void * /*outputBuffer*/, void *inputBuffer, unsigned int nFrames, double /*streamTime*/, RtAudioStreamStatus /*status*/, void * /*userData*/) {
    bool end = false;
    get().dataCallback(static_cast<float *>(inputBuffer), nFrames, &end);
    return end;
}

void AudioCaptureManager::initCallback(int channels, int samplesPerSecond) {
    this->channels = channels;
    this->samplesPerSecond = samplesPerSecond;
    this->samplesPerFrame = samplesPerSecond / 100;
    if (GUI::Colorplot::getLast()) {
        GUI::Colorplot::getLast()->setBlockSize(512);
    }
}

void AudioCaptureManager::dataCallback(float* data, unsigned int frames, bool*done){    
    *done = !run;
    if (!run) {
        return;
    }
    if(!data)
        return;
    if(channels<0)
        return;
    int firstIndex = -1;
    for (int i = 0; i < channels; ++i) {
        if(data[i]!=0){
            firstIndex=i;
            break;
        }
    }
    if(firstIndex==-1)
        return;

    sample.addData(data,data+frames*static_cast<unsigned>(channels),channels-1,firstIndex);

    audiofft.analyse(sample.data(),1,fftoutput.data());
    {
        // feed the *analysis classes with new samples
        int restFrames = static_cast<int>(frames);
        if (restFrames % samplesPerFrame != 0) {
            static bool once = false;
            if (!once) {
                once = true;
                ErrorNotifier::showError(QStringLiteral("The samples from the audio capture service does not have a length of %1 or x * %1. The length is %2. Can not analyse audio data.").arg(samplesPerFrame).arg(frames));
            }
        } else {
            while (restFrames != 0) {
                if (restFrames >= sample.size()) {
                    // we have to ignore some data
                    restFrames -= samplesPerFrame;
                    continue;
                }
                for (auto &[onsetFunction, pair] : onsetAnalyzes) {
                    bool wasOnset = pair.first.processNewSamples(sample.data() + sample.size() - restFrames);
                    pair.second.addOnsetData(pair.second.getNewestSample(), pair.first.getOnsetValue(), 0);
                    if (wasOnset) {
                        pair.second.addEvent(pair.first.getLastOnset());
                    }
                    pair.second.increaseNewestSampleBy(samplesPerFrame);
                }
                for (auto &[onsetFunction, pair] : tempoAnalyzes) {
                    bool wasBeat = pair.first.processNewSamples(sample.data() + sample.size() - restFrames);
                    if (wasBeat) {
                        pair.second.addEvent(pair.first.getLastBeat());
                    }
                    pair.second.increaseNewestSampleBy(samplesPerFrame);
                }
                restFrames -= samplesPerFrame;
            }
        }
    }
    // db scale
    std::transform(fftoutput.begin(), fftoutput.end(), fftoutput.begin(), [](auto i) { return 10 * std::log10(1 + i); });

    if (GUI::Graph::getLast() && run) {
        GUI::Graph::getLast()->showData(fftoutput.data(), fftoutput.size());
    }
    if (GUI::Colorplot::getLast() && run) {
        GUI::Colorplot::getLast()->startBlock();
        for (int i = 0; i < 512; ++i) {
            GUI::Colorplot::getLast()->pushDataToBlock(fftoutput.at(i));
        }
        GUI::Colorplot::getLast()->endBlock();
    }
    if (GUI::Oscillogram::getLast() && run) {
        GUI::Oscillogram::getLast()->showData(sample.data(), sample.size());
    }
}

void AudioCaptureManager::startCapturingFromCaptureLibrary(AudioCaptureManager::CaptureLibEntry func) {
    captureAudioThread = std::thread([this, func]() {
        run = true;
        emit this->capturingStatusChanged();
        func(&AudioCaptureManager::staticInitCallback, &AudioCaptureManager::staticDataCallback);
        run = false;
        emit this->capturingStatusChanged();
    });
}

bool AudioCaptureManager::startCapturingFromInput(unsigned input) {
    if (input >= rtAudio.getDeviceCount()) {
        return false;
    }
    const auto di = rtAudio.getDeviceInfo(input);
    if (di.inputChannels == 0) {
        return false;
    }
    // it was a device open before now
    int sampleRate = this->samplesPerSecond;
    if (this->samplesPerSecond > 0) {
        bool supported = std::any_of(di.sampleRates.cbegin(), di.sampleRates.cend(), [this](int s) { return s == this->samplesPerSecond; });
        if (!supported) {
            return false;
        }
    } else {
        sampleRate = static_cast<int>(di.preferredSampleRate);
    }
    initCallback(static_cast<int>(di.inputChannels), sampleRate);

    RtAudio::StreamParameters isp;
    isp.deviceId = input;
    isp.nChannels = di.inputChannels;
    isp.firstChannel = 0;
    unsigned samplesPerFrame = static_cast<unsigned>(this->samplesPerFrame);
    try {
        rtAudio.openStream(nullptr, &isp, RTAUDIO_FLOAT32, static_cast<unsigned>(this->samplesPerSecond), &samplesPerFrame, rtAudioCallback, nullptr, nullptr, nullptr);
        if (static_cast<int>(samplesPerFrame) != this->samplesPerFrame) {
            rtAudio.closeStream();
            return false;
        }
        rtAudio.startStream();
        run = true;
        emit this->capturingStatusChanged();
    } catch (const RtAudioError &error) {
        ErrorNotifier::showError(QString::fromStdString(error.getMessage()));
        run = false;
        emit capturingStatusChanged();
        return false;
    }
    return true;
}

bool AudioCaptureManager::loadCaptureLibrary(QString name, QString filePathToCaptureLibrary) {
    auto func = reinterpret_cast<CaptureLibEntry>(QLibrary::resolve(filePathToCaptureLibrary, "captureAudio"));
    if (func) {
        // replace if name is already there
        auto res = captureLibraries.emplace(name, func);
        if (res.second) {
            auto pos = std::distance(captureLibraries.begin(), res.first);
            captureDeviceNames.getVector().insert(captureDeviceNames.cbegin() + pos, name);
            if (currentCaptureDevice >= pos) {
                currentCaptureDevice++;
                emit currentCaptureDeviceChanged();
            }
        }
    }
    return func;
}

bool AudioCaptureManager::startCapturingFromDevice(const QString &name) {
    for (const auto &c : captureLibraries) {
        if (c.first == name) {
            startCapturingFromCaptureLibrary(c.second);
            return true;
        }
    }
    for (unsigned i = 0; i < rtAudio.getDeviceCount(); ++i) {
        if (auto di = rtAudio.getDeviceInfo(i); di.name.c_str() == name) {
            return startCapturingFromInput(i);
        }
    }
    return false;
}

bool AudioCaptureManager::startCapturingFromCaptureLibrary() {
    if (captureLibraries.empty()) {
        return false;
    }
    stopCapturingAndWait();
    startCapturingFromCaptureLibrary(captureLibraries.begin()->second);
    currentCaptureDevice = 0;
    emit currentCaptureDeviceChanged();
    return true;
}

bool AudioCaptureManager::startCapturingFromDefaultInput() {    
    stopCapturingAndWait();
    // check if default input is availible
    const auto input = rtAudio.getDefaultInputDevice();
    if (input >= rtAudio.getDeviceCount()) {
        return false;
    }
    const auto di = rtAudio.getDeviceInfo(input);
    if (!di.isDefaultInput) {
        return false;
    }
    if (startCapturingFromInput(input)) {
        currentCaptureDevice = getIndexForDeviceName(di.name.c_str());
        emit currentCaptureDeviceChanged();
        return true;
    }
    return false;
}

void AudioCaptureManager::stopCapturing() {
    try {
        if (rtAudio.isStreamRunning()) {
            rtAudio.abortStream();
        }
        if (rtAudio.isStreamOpen()) {
            rtAudio.closeStream();
        }
    } catch (const RtAudioError &e) {
        ErrorNotifier::showError("Error while stopping audio stream: " + QString(e.what()));
    }
    run = false;
    emit capturingStatusChanged();
}

void AudioCaptureManager::stopCapturingAndWait() {
    try {
        if (captureAudioThread.joinable()) {
            run = false;
            captureAudioThread.join();
        } else {
            if (rtAudio.isStreamOpen()) {
                rtAudio.closeStream();
                std::this_thread::yield();
                while (rtAudio.isStreamRunning()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(500));
                }
            }
            run = false;
        }
    } catch (const RtAudioError &e) {
        ErrorNotifier::showError("Error while stopping audio stream: " + QString(e.what()));
    }
    emit capturingStatusChanged();
}

bool AudioCaptureManager::isCapturing() const {
    return run || rtAudio.isStreamRunning();
}

void AudioCaptureManager::updateCaptureDeviceList() {
    QString name;
    if (currentCaptureDevice >= 0 && currentCaptureDevice < captureDeviceNames.ssize()) {
        name = captureDeviceNames[currentCaptureDevice];
    }
    captureDeviceNames.clear();
    for (const auto &i : captureLibraries) {
        captureDeviceNames.push_back(i.first);
    }

    for (unsigned i = 0; i < rtAudio.getDeviceCount(); ++i) {
        if (auto di = rtAudio.getDeviceInfo(i); di.inputChannels > 0) {
            captureDeviceNames.emplace_back(QString::fromStdString(di.name.c_str()));
        }
    }
    auto newIndex = getIndexForDeviceName(name);
    if (newIndex != currentCaptureDevice) {
        currentCaptureDevice = newIndex;
        emit currentCaptureDeviceChanged();
    }
}

void AudioCaptureManager::setCurrentCaptureDevice(int index) {
    if (index != currentCaptureDevice) {
        if (index < 0 || index >= captureDeviceNames.ssize()) {
            stopCapturing();
            currentCaptureDevice = -1;
            emit currentCaptureDeviceChanged();
            return;
        }
        stopCapturingAndWait();
        currentCaptureDevice = index;
        if (!startCapturingFromDevice(captureDeviceNames[index])) {
            ErrorNotifier::showError("Error while starting stream " + captureDeviceNames[index]);
        }
        emit currentCaptureDeviceChanged();
    }
}

const EventSeries *AudioCaptureManager::requestTempoAnalysis(Aubio::OnsetDetectionFunction f) {
    if (samplesPerSecond < 0) {
        return nullptr;
    }
    // check if already there
    if (const auto i = tempoAnalyzes.find(f); i != tempoAnalyzes.end()) {
        return &i->second.second;
    }
    // We need this ugly syntax, because we can not copy or move a EventRange object. See https://stackoverflow.com/a/25767752/10162645
    return &tempoAnalyzes.emplace(std::piecewise_construct, std::make_tuple(f), std::forward_as_tuple(std::piecewise_construct, std::forward_as_tuple(f, 1024, samplesPerFrame, samplesPerSecond), std::forward_as_tuple(samplesPerSecond))).first->second.second;
    // short:  tempoAnalyzes.emplace(f, {Aubio::TempoAnalysis(f, 1024, 441, 44100), OnsetDataSeries(44100)});
}

const OnsetDataSeries *AudioCaptureManager::requestOnsetAnalysis(Aubio::OnsetDetectionFunction f) {
    if (samplesPerSecond < 0) {
        return nullptr;
    }
    // check if already there
    if (const auto i = onsetAnalyzes.find(f); i != onsetAnalyzes.end()) {
        return &i->second.second;
    }
    // We need this ugly syntax, because we can not copy or move a EventRange object. See https://stackoverflow.com/a/25767752/10162645
    return &onsetAnalyzes.emplace(std::piecewise_construct, std::make_tuple(f), std::forward_as_tuple(std::piecewise_construct, std::forward_as_tuple(f, 1024, samplesPerFrame, samplesPerSecond), std::forward_as_tuple(samplesPerSecond))).first->second.second;
    // short:  onsetAnalyzes.emplace(f, {Aubio::OnsetAnalysis(f, 1024, 441, 44100), OnsetDataSeries(44100)});
}

} // namespace Audio
