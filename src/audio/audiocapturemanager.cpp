#include "audiocapturemanager.h"
#include "errornotifier.h"
#include "gui/colorplot.h"
#include "gui/graph.h"
#include "gui/oscillogram.h"
#include <algorithm>

namespace Audio {
AudioCaptureManager::AudioCaptureManager():audiofft(sample.size())
{

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
        unsigned restFrames = frames;
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
    std::transform(fftoutput.begin(),fftoutput.end(),fftoutput.begin(),[](auto i){return 10*std::log10(1+i);});

    if(GUI::Graph::getLast())
        GUI::Graph::getLast()->showData(fftoutput.data(),fftoutput.size());
    if(GUI::Colorplot::getLast()){
        GUI::Colorplot::getLast()->startBlock();
        for (int i = 0; i < 512; ++i) {
            GUI::Colorplot::getLast()->pushDataToBlock(fftoutput.at(i));
        }
        GUI::Colorplot::getLast()->endBlock();
    }
    if(GUI::Oscillogram::getLast())
        GUI::Oscillogram::getLast()->showData(sample.data(),sample.size());

}

bool AudioCaptureManager::startCapturing(QString filePathToCaptureLibrary){
    stopCapturingAndWait();
    typedef int (*capture)(void (*)(int, int), void (*)(float *, unsigned int, bool *));
    auto func = reinterpret_cast<capture>(QLibrary::resolve(filePathToCaptureLibrary,"captureAudio"));
    if(func){
        captureAudioThread = std::thread([this,func](){
            run = true;
            emit this->capturingStatusChanged();
            func(&AudioCaptureManager::staticInitCallback,&AudioCaptureManager::staticDataCallback);
            run = false;
            emit this->capturingStatusChanged();
        });
    }
    return func;
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
