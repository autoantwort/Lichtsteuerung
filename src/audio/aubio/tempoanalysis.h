#ifndef TEMPOANALYSIS_H
#define TEMPOANALYSIS_H

#include "aubiocapi.h"
#include <memory>

namespace Audio::Aubio {

/**
 * @brief The TempoAnalysis class can determine the tempo (beats per minute) of the input audio data. Also the class can detect beats.
 * The class can use different onset detection methods (onset = "music event"), which leads to different results.
 */
class TempoAnalysis {
    std::unique_ptr<C_API::aubio_tempo_t, decltype(&C_API::del_aubio_tempo)> tempo;
    C_API::fvec_t inputData;

public:
    /**
     * @brief TempoAnalysis Creates a tempo analysis, which can detect beats, tatums and the current bpm
     * @param onsetDetectionFunction The onset detection function that should be used. The results can vary greatly based on the onset detection function
     * @param fftSize The size of the fft that should be used. Larger fft sizes are slower but result in better results (must be a power of 2)
     * @param stepSize The size of a new block that gets passed to this->processNewSamples().
     * @param sampleRate The sample rate of the data
     */
    TempoAnalysis(OnsetDetectionFunction onsetDetectionFunction, C_API::uint_t fftSize, C_API::uint_t stepSize, C_API::uint_t sampleRate);

    /**
     * @brief processNewSamples process and analyse new samples
     * @param newSamples A pointer to the new samples, the length of the array must be minimun the step size from the constructor
     * @return true, if there was a beat in the new sample, false otherwise
     */
    bool processNewSamples(float *newSamples);

    /**
     * @brief getLastBeat returns the position of the last detected beat, in samples
     * @return the position of the last beat in samples
     */
    unsigned getLastBeat();

    /**
     * @brief getCurrentTempoConfidence returns the current confidence about the tempo
     * @return the confidence from 0.0 (no) to 1.0 (high)
     */
    float getCurrentTempoConfidence();

    /**
     * @brief getCurrentBPM return the current determined beats per second
     * @return the currently determined beats per minute
     */
    float getCurrentBPM();

    /**
     * @brief getLastTatum return the position of the last detected tatum (sub beat) in samples
     * @return the position of the last tatum im samples
     */
    unsigned getLastTatum();
};

} // namespace Audio::Aubio

#endif // TEMPOANALYSIS_H
