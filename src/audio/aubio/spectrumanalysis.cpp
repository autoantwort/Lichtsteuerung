#include "spectrumanalysis.h"

namespace Audio::Aubio {

using namespace C_API;

SpectrumAnalysis::SpectrumAnalysis(C_API::uint_t spectrumSize, C_API::uint_t stepSize)
    : spectrumAnalyser(new_aubio_pvoc(spectrumSize * 2, stepSize), del_aubio_pvoc), fftOutputData(new_cvec(spectrumSize * 2), del_cvec), inputData{stepSize, nullptr} {}

void SpectrumAnalysis::processNewSamples(float *newSamples) {
    inputData.data = newSamples;
    aubio_pvoc_do(spectrumAnalyser.get(), &inputData, fftOutputData.get());
}

span<float> SpectrumAnalysis::getSpectrum() const {
    return {fftOutputData->norm, fftOutputData->length};
}

float *SpectrumAnalysis::getPointerToSpectrum() const {
    return fftOutputData->norm;
}

} // namespace Audio::Aubio
