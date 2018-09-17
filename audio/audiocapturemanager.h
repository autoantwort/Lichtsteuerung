#ifndef AUDIOCAPTUREMANAGER_H
#define AUDIOCAPTUREMANAGER_H

#include "sample.h"
#include <thread>
#include "audio_fft.h"

namespace Audio {

/**
 * @brief The AudioCaptureManager class gets the data from the captureWindowsSountoutput Project and analyse the data and give the data to the other components
 */
class AudioCaptureManager
{
    Sample<float,4096> sample;
    std::array<float,2048> fftoutput;
    std::thread captureAudioThread;
    std::atomic_bool run;
    AudioFFT audiofft;
    int channels = -1;

private:
    AudioCaptureManager();
    ~AudioCaptureManager(){
        if(captureAudioThread.joinable()){
            run.store(false);
            captureAudioThread.join();
        }
    }
private:
    static void staticInitCallback(int channels){get().initCallback(channels);}
    static void staticDataCallback(float* data, unsigned int frames, bool*done){get().dataCallback(data,frames,done);}
    void initCallback(int channels);
    void dataCallback(float* data, unsigned int frames, bool*done);
public:
    bool startCapturing(QString filePathToCaptureLibrary);
public:
    AudioCaptureManager(AudioCaptureManager const&)               = delete;
    void operator=(AudioCaptureManager const&)  = delete;
    static AudioCaptureManager & get(){static AudioCaptureManager m;return m;}
};

}

#endif // AUDIOCAPTUREMANAGER_H
