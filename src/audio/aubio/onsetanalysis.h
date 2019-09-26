#ifndef ONSETANALYSIS_H
#define ONSETANALYSIS_H

#include "aubiocapi.h"
#include <memory>

namespace Audio::Aubio {

/**
 * @brief The OnsetAnalysis class can detect onsets (music events). They are similar to the beats, but not so uniformly (rather the feeling of the music).
 */
class OnsetAnalysis {
    std::unique_ptr<C_API::aubio_onset_t, decltype(&C_API::del_aubio_onset)> onset;
    C_API::fvec_t inputData;

public:
    /**
     * @brief OnsetAnalysis Creates a onset analysis, which can detect onsets (music events)
     * @param onsetDetectionFunction The onset detection function that should be used. The results can vary greatly based on the onset detection function
     * @param fftSize The size of the fft that should be used. Larger fft sizes are slower but result in better results (must be a power of 2)
     * @param stepSize The size of a new block that gets passed to this->processNewSamples().
     * @param sampleRate The sample rate of the data
     */
    OnsetAnalysis(OnsetDetectionFunction onsetDetectionFunction, C_API::uint_t fftSize, C_API::uint_t stepSize, C_API::uint_t sampleRate);

    /**
     * @brief processNewSamples process and analyse new samples
     * @param newSamples A pointer to the new samples, the length of the array must be minimun the step size from the constructor
     * @return true, if there was a onset in the new sample, false otherwise
     */
    bool processNewSamples(float *newSamples);

    /**
     * @brief getLastOnset return the last detected onset
     * @return the position of the last detected onset in samples
     */
    unsigned getLastOnset();

    /**
     * @brief getOnsetValue return the last value of the onset detection function that is used
     * @return the latest value of the onset detection function, the value range can vary from onset detection function to onset detection function
     */
    float getOnsetValue();

    /**
     * @brief getCurrentThreshold return the current threshold used by the detection function to detect onsets
     * @return the current threshold used by the detection function to detect onsets
     */
    float getCurrentThreshold();
};

} // namespace Audio::Aubio

#endif // ONSETANALYSIS_H
