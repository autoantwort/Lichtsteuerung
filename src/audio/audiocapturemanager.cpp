#include "audiocapturemanager.h"
#include "gui/graph.h"
#include "gui/colorplot.h"
#include "gui/oscillogram.h"
#include <algorithm>

namespace Audio {
AudioCaptureManager::AudioCaptureManager():audiofft(sample.size())
{

}

void AudioCaptureManager::initCallback(int channels){
    this->channels = channels;
    if(GUI::Colorplot::getLast())
        GUI::Colorplot::getLast()->setBlockSize(512);
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

    //db scale
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
    typedef int (*capture)(void(*)(int),void(*)(float*,unsigned int, bool*)) ;
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



}

