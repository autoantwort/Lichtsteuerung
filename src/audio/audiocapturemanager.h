#ifndef AUDIOCAPTUREMANAGER_H
#define AUDIOCAPTUREMANAGER_H

#include "audioeventdata.h"
#include "modelvector.h"
#include "sample.h"
#include "span.h"
#include "aubio/onsetanalysis.h"
#include "aubio/spectrumanalysis.h"
#include "aubio/tempoanalysis.h"
#include <map>
#include <optional>
#include <rtaudio/RtAudio.h>
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

struct TempoAnalysisData {
    const EventSeries *events;
    const float *confidence;
    operator bool() { return events != nullptr; }
};

/**
 * @brief The AudioCaptureManager class gets the data from the captureWindowsSountoutput Project and analyse the data and give the data to the other components
 */
class AudioCaptureManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool capturing READ isCapturing NOTIFY capturingStatusChanged)
    Q_PROPERTY(int currentCaptureDevice READ getCurrentCaptureDevice WRITE setCurrentCaptureDevice NOTIFY currentCaptureDeviceChanged)
    Q_PROPERTY(QAbstractItemModel *captureDeviceNames READ getCaptureDeviceNamesModel CONSTANT)
    Sample<float, 4096> sample;
    std::atomic_bool run;
    int currentCaptureDevice = -1;
    RtAudio rtAudio;
    ModelVector<QString> captureDeviceNames;
    // we use a optional to delay the creation of the object until we know the samplesPerFrame value
    std::optional<Aubio::SpectrumAnalysis> spectrumAnalysis;
    static constexpr int SPECTRUM_BUCKET_COUNT = 2048;
    std::array<float, SPECTRUM_BUCKET_COUNT + 1> spectrumLogarithmic;

    int channels = -1;
    int samplesPerSecond = -1;
    int samplesPerFrame = -1;

    struct TempoAnalysis {
        Aubio::TempoAnalysis tempoAnalysis;
        EventSeries events;
        float currentConfidence;
        TempoAnalysis(Aubio::OnsetDetectionFunction onsetDetectionFunction, int fftSize, int stepSize, int sampleRate)
            : tempoAnalysis(onsetDetectionFunction, fftSize, stepSize, sampleRate)
            , events(sampleRate)
            , currentConfidence(0) {}
        operator TempoAnalysisData() { return {&events, &currentConfidence}; }
    };
    /**
     * @brief tempoAnalyzes all tempo analyzes that were request by requestTempoAnalysis
     */
    std::map<Aubio::OnsetDetectionFunction, TempoAnalysis> tempoAnalyzes;
    /**
     * @brief onsetAnalyzes all onset analyzes that were request by requestOnsetAnalysis
     */
    std::map<Aubio::OnsetDetectionFunction, std::pair<Aubio::OnsetAnalysis, OnsetDataSeries>> onsetAnalyzes;

private:
    AudioCaptureManager();

private:
    static void rtAudioErrorCallback(RtAudioError::Type type, const std::string &errorText);
    static int rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData);
    void initCallback(int channels, int samplesPerSecond);
    void dataCallback(float *data, unsigned int frames, bool *done);

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
     * @brief startCapturingFromDevice starts the capturing with a capture device with the given name. For the names, see getCaptureDeviceNames()
     * @param name the name of the capture device
     * @return true, if the capturing stats successfully, false otherwise
     */
    Q_INVOKABLE bool startCapturingFromDevice(const QString &name);
    /**
     * @brief startCapturingFromDefaultInput starts the capturing from the default input device. On windows from the default output.
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

    span<float> getFFTOutput() { return (spectrumAnalysis ? spectrumAnalysis->getSpectrum() : span<float>{nullptr, 0}); }
    /**
     * @brief requestTempoAnalysis requests the data series from a tempo analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Event Series produced by the analysis object using the specific onset detection function and a confidence level between 0 (no) and 1 (high)
     */
    TempoAnalysisData requestTempoAnalysis(Aubio::OnsetDetectionFunction f);
    /**
     * @brief requestOnsetAnalysis requests the data series from a onset analysis that uses a spezific onset detection function
     * You can call the function with the same parameters multiple times, the result will be the same
     * @param f the onset function that should be used
     * @return the Onset Data Series produced by the analysis object using the specific onset detection function
     */
    const OnsetDataSeries *requestOnsetAnalysis(Aubio::OnsetDetectionFunction f);

public:
    AudioCaptureManager(AudioCaptureManager const &) = delete;
    void operator=(AudioCaptureManager const &) = delete;
    static AudioCaptureManager &get() {
        static AudioCaptureManager m;
        return m;
    }
signals:
    void capturingStatusChanged();
    void currentCaptureDeviceChanged();
    /**
     * @brief capturingStarted is emmited when the capturing really starts (the init callback is called).
     * You now should update your reference from the getFFTOutput() function
     */
    void capturingStarted();
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
