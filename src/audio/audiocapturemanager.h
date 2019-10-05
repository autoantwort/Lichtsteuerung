#ifndef AUDIOCAPTUREMANAGER_H
#define AUDIOCAPTUREMANAGER_H

#include "audioeventdata.h"
#include "modelvector.h"
#include "sample.h"
#include "aubio/onsetanalysis.h"
#include "aubio/tempoanalysis.h"
#include "audio_fft.h"
#include "lib/RtAudio/rtaudio/RtAudio.h"
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
    Q_PROPERTY(int currentCaptureDevice READ getCurrentCaptureDevice WRITE setCurrentCaptureDevice NOTIFY currentCaptureDeviceChanged)
    Q_PROPERTY(QAbstractItemModel *captureDeviceNames READ getCaptureDeviceNamesModel CONSTANT)
    Sample<float,4096> sample;
    std::array<float,2048> fftoutput;
    std::thread captureAudioThread;
    std::atomic_bool run;
    int currentCaptureDevice = -1;
    RtAudio rtAudio;
    AudioFFT audiofft;
    ModelVector<QString> captureDeviceNames;
    using CaptureLibEntry = int (*)(void(int, int), void(float *, unsigned int, bool *));
    /**
     * @brief captureLibraries all libraries loaded with loadCaptureLibrary. The string contains the name and the CaptureLibEntry the entry funciton
     */
    std::map<QString, CaptureLibEntry> captureLibraries;

    int channels = -1;
    int samplesPerSecond = -1;
    int samplesPerFrame = -1;
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
    static int rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData);
    static void staticInitCallback(int channels, int samplesPerSecond) { get().initCallback(channels, samplesPerSecond); }
    static void staticDataCallback(float* data, unsigned int frames, bool*done){get().dataCallback(data,frames,done);}
    void initCallback(int channels, int samplesPerSecond);
    void dataCallback(float *data, unsigned int frames, bool *done);

    /**
     * @brief startCapturingFromCaptureLibrary starts the audio capturing with the given function
     * @param func The entry function of the audio capture lib
     */
    void startCapturingFromCaptureLibrary(CaptureLibEntry func);

    /**
     * @brief startCapturingFromInput starts the captuing from an input device
     * @param inputIndex The index of the input device from rtAudio.getDeviceInfo(...)
     * @return true if the starting of the capturing was successful, false otherwise
     */
    bool startCapturingFromInput(unsigned inputIndex);

    /**
     * @brief getIndexForDeviceName returns the index in the captureDeviceNames for the device with the given name
     * @param name the device name
     * @return the index of the device in the captureDeviceNames, or -1, if there is no device with the given name
     */
    template <typename String>
    int getIndexForDeviceName(const String &name);

public:
    /**
     * @brief loadCaptureLibrary loads the library located at the given path with a given name, with this name you can start capturing. The list can be updated with updateCaptureDeviceList()
     * @param name the name that the capture device should have, if there is already a device with the name, replace this
     * @param filePathToCaptureLibrary The path to the capture lib
     * @return true, if the loading was successfull, false otherwise
     */
    bool loadCaptureLibrary(QString name, QString filePathToCaptureLibrary);

    /**
     * @brief startCapturingFromDevice starts the capturing with a capture device with the given name. For the names, see getCaptureDeviceNames()
     * @param name the name of the capture device
     * @return true, if the capturing stats successfully, false otherwise
     */
    Q_INVOKABLE bool startCapturingFromDevice(const QString &name);
    /**
     * @brief startCapturingFromCaptureLibrary starts the capturing from one capture library that was loaded with loadCaptureLibrary(...), which one is undefined
     * @return true, if the capturing stats successfully, false otherwise
     */
    bool startCapturingFromCaptureLibrary();
    /**
     * @brief startCapturingFromDefaultInput starts the capturing from the default input device
     * @return true, if the capturing stats successfully, false otherwise
     */
    bool startCapturingFromDefaultInput();
    void stopCapturing();
    void stopCapturingAndWait();
    bool isCapturing() const;
    /**
     * @brief updateCaptureDeviceList updates the list of devices from which capturing can be started. See getCaptureDeviceNames()
     */
    Q_INVOKABLE void updateCaptureDeviceList();
    int getCurrentCaptureDevice() const { return currentCaptureDevice; }
    void setCurrentCaptureDevice(int index);

    QAbstractItemModel *getCaptureDeviceNamesModel() { return &captureDeviceNames; }
    /**
     * @brief getCaptureDeviceNames returns the name of all devices from which capturing can be started. The list can be updated with updateCaptureDeviceList()
     * @return a name list of all devices from which capturing can be started
     */
    const std::vector<QString> &getCaptureDeviceNames() const { return captureDeviceNames.getVector(); }

    const std::array<float, 2048> &getFFTOutput() { return fftoutput; }
    /**
     * @brief requestTempoAnalysis requests the data series from a tempo analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Event Series produced by the analysis object using the specific onset detection function
     */
    const EventSeries *requestTempoAnalysis(Aubio::OnsetDetectionFunction f);
    /**
     * @brief requestOnsetAnalysis requests the data series from a onset analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Onset Data Series produced by the analysis object using the specific onset detection function
     */
    const OnsetDataSeries *requestOnsetAnalysis(Aubio::OnsetDetectionFunction f);

public:
    AudioCaptureManager(AudioCaptureManager const&)               = delete;
    void operator=(AudioCaptureManager const&)  = delete;
    static AudioCaptureManager & get(){static AudioCaptureManager m;return m;}
signals:
    void capturingStatusChanged();
    void currentCaptureDeviceChanged();
};

template <typename String>
int AudioCaptureManager::getIndexForDeviceName(const String &name) {
    int index = 0;
    for (const auto &n : captureDeviceNames) {
        if (n == name) {
            return index;
        }
        ++index;
    }
    return -1;
}

} // namespace Audio

#endif // AUDIOCAPTUREMANAGER_H
