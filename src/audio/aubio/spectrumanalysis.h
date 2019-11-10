#ifndef SPECTRUMANALYSIS_H
#define SPECTRUMANALYSIS_H

#include "aubiocapi.h"
#include <boost/beast/core/span.hpp> // TODO use std::span in c++20
#include <memory>

namespace Audio::Aubio {

/**
 * @brief The SpectrumAnalysis class computes a spectrum (energy for every frequency) for the audio data.
 */
class SpectrumAnalysis {
    std::unique_ptr<C_API::aubio_pvoc_t, decltype(&C_API::del_aubio_pvoc)> spectrumAnalyser;
    std::unique_ptr<C_API::cvec_t, decltype(&C_API::del_cvec)> fftOutputData;
    C_API::fvec_t inputData;

public:
    /**
     * @brief SpectrumAnalysis Creates a spectrum analysis.
     * @param spectrumSize The size of blocks in the spectrum from 0 to sampleRate/2. Must be a power of 2.
     * @param stepSize The size of a new block that gets passed to this->processNewSamples().
     */
    SpectrumAnalysis(C_API::uint_t spectrumSize, C_API::uint_t stepSize);

    /**
     * @brief processNewSamples process and analyse new samples
     * @param newSamples A pointer to the new samples, the length of the array must be minimun the step size from the constructor
     */
    void processNewSamples(float *newSamples);

    /**
     * @brief getSpectrum returns the spectrum (the enery for every block)
     * @return the spectrum
     */
    [[nodiscard]] boost::beast::span<float> getSpectrum() const;

    /**
     * @brief getPointerToSpectrum See getSpectrum().
     * @return A pointer to the spectrum. You can modify the values. They will be overwritten in the processNewSamples(float*) method
     */
    [[nodiscard]] float *getPointerToSpectrum() const; // remove when using std::span in c++20
};

} // namespace Audio::Aubio

#endif // SPECTRUMANALYSIS_H
