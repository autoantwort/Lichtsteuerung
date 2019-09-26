#ifndef AUDIOCAPTUREMANAGER_H
#define AUDIOCAPTUREMANAGER_H

#include "aubio/onsetanalysis.h"
#include "aubio/tempoanalysis.h"
#include "audio_fft.h"
#include "audioeventdata.h"
#include "sample.h"
#include <map>
#include <thread>

namespace Audio {

/*
 * ## Audio introduction
 * The latest Aubio (music analysing lib) documentation can be found here: https://aubio.org/doc/latest/index.html
 * You can look here if you want to know how to use the lib: https://github.com/aubio/vamp-aubio-plugins/tree/master/plugins
 *
 * We normally get 44100 samples per second in blocks of 441 every 10 ms (100 blocks per second).
 * The duration of one sample is then ~0,0226 ms. If you have 200 bpm, you have a beat every 60*1000/200 = 300ms or every 44100*60/200 = 13'230 samples.
 * The hop/step size of a algorithm is the size of the data block, that the algorithm gets in every iteration (in the above example 441).
 * If you get events at sample positions with type int, you will get an overflow after 2^31/44100/60/60 = 13.5 hours.
 *
 */

/**
 * @brief The AudioCaptureManager class gets the data from the captureWindowsSountoutput Project and analyse the data and give the data to the other components
 */
class AudioCaptureManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool capturing READ isCapturing NOTIFY capturingStatusChanged)
    Sample<float,4096> sample;
    std::array<float,2048> fftoutput;
    std::thread captureAudioThread;
    std::atomic_bool run;
    AudioFFT audiofft;
    int channels = -1;
    /**
     * @brief tempoAnalyzes all tempo analyzes that were request by requestTempoAnalysis
     */
    std::map<Aubio::OnsetDetectionFunction, std::pair<Aubio::TempoAnalysis, EventSeries>> tempoAnalyzes;
    /**
     * @brief onsetAnalyzes all onset analyzes that were request by requestOnsetAnalysis
     */
    std::map<Aubio::OnsetDetectionFunction, std::pair<Aubio::OnsetAnalysis, OnsetDataSeries>> onsetAnalyzes;

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
    void stopCapturing(){run=false;}
    void stopCapturingAndWait(){run=false;if(captureAudioThread.joinable())captureAudioThread.join();}
    bool isCapturing(){return run;}
    const std::array<float, 2048> &getFFTOutput() { return fftoutput; }
    /**
     * @brief requestTempoAnalysis requests the data series from a tempo analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Event Series produced by the analysis object using the specific onset detection function
     */
    const EventSeries &requestTempoAnalysis(Aubio::OnsetDetectionFunction f);
    /**
     * @brief requestOnsetAnalysis requests the data series from a onset analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Onset Data Series produced by the analysis object using the specific onset detection function
     */
    const OnsetDataSeries &requestOnsetAnalysis(Aubio::OnsetDetectionFunction f);

public:
    AudioCaptureManager(AudioCaptureManager const&)               = delete;
    void operator=(AudioCaptureManager const&)  = delete;
    static AudioCaptureManager & get(){static AudioCaptureManager m;return m;}
signals:
    void capturingStatusChanged();
};

} // namespace Audio

#endif // AUDIOCAPTUREMANAGER_H
