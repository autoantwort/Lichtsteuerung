#include "tempoanalysis.h"
#include <QDebug>
#include <array>

namespace Audio::Aubio {

using namespace C_API;

TempoAnalysis::TempoAnalysis(OnsetDetectionFunction onsetDetectionFunction, uint_t fftSize, uint_t stepSize, uint_t sampleRate) : tempo(new_aubio_tempo(toName(onsetDetectionFunction), fftSize, stepSize, sampleRate), del_aubio_tempo), inputData{stepSize, nullptr} {}

bool TempoAnalysis::processNewSamples(float *newSamples) {
    // setup the data structures
    smpl_t wasBeat;
    fvec_t out{1, &wasBeat};
    inputData.data = newSamples;

    aubio_tempo_do(tempo.get(), &inputData, &out);

    return wasBeat != 0;
}

unsigned TempoAnalysis::getLastBeat() { return aubio_tempo_get_last(tempo.get()); }

float TempoAnalysis::getCurrentBPM() { return aubio_tempo_get_bpm(tempo.get()); }

unsigned TempoAnalysis::getLastTatum() { return static_cast<unsigned>(aubio_tempo_get_last_tatum(tempo.get())); }

} // namespace Audio::Aubio
