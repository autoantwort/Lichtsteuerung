#include "onsetanalysis.h"

#include <QDebug>
#include <array>

namespace Audio::Aubio {

using namespace C_API;

OnsetAnalysis::OnsetAnalysis(OnsetDetectionFunction onsetDetectionFunction, uint_t fftSize, uint_t stepSize, uint_t sampleRate) : onset(new_aubio_onset(toName(onsetDetectionFunction), fftSize, stepSize, sampleRate), del_aubio_onset), inputData{stepSize, nullptr} {}

bool OnsetAnalysis::processNewSamples(float *newSamples) {
    // setup the data structures
    smpl_t wasOnset;
    fvec_t out{1, &wasOnset};
    inputData.data = newSamples;

    aubio_onset_do(onset.get(), &inputData, &out);

    return wasOnset != 0;
}

unsigned OnsetAnalysis::getLastOnset() { return aubio_onset_get_last(onset.get()); }

float OnsetAnalysis::getOnsetValue() { return aubio_onset_get_descriptor(onset.get()); }

float OnsetAnalysis::getCurrentThreshold() { return aubio_onset_get_thresholded_descriptor(onset.get()); }

} // namespace Audio::Aubio
