#include "aubiocapi.h"
#include <QString>
#include <QtGlobal>

namespace Audio::Aubio {

OnsetDetectionFunction toOnsetDetectionFunction(int i) {
    Q_ASSERT(i >= 0 && i <= static_cast<int>(OnsetDetectionFunction::Last));
    return static_cast<OnsetDetectionFunction>(i);
}

const C_API::char_t *C_API::toName(OnsetDetectionFunction f) {
    using E = OnsetDetectionFunction;
    switch (f) {
    case E::EnergyBased: return "energy";
    case E::SpectralDifference: return "specdiff";
    case E::HighFrequencyContent: return "hfc";
    case E::ComplexDomain: return "complex";
    case E::PhaseDeviation: return "phase";
    case E::KullbackLiebler: return "kl";
    case E::ModifiedKullbackLiebler: return "mkl";
    case E::SpectralFlux: return "specflux";
    }
}

QString toQString(OnsetDetectionFunction f) {
    using E = OnsetDetectionFunction;
    switch (f) {
    case E::EnergyBased: return QStringLiteral("Energy Based");
    case E::SpectralDifference: return QStringLiteral("Spectral Difference");
    case E::HighFrequencyContent: return QStringLiteral("High-Frequency Content");
    case E::ComplexDomain: return QStringLiteral("Complex Domain");
    case E::PhaseDeviation: return QStringLiteral("Phase Deviation");
    case E::KullbackLiebler: return QStringLiteral("Kullback-Liebler");
    case E::ModifiedKullbackLiebler: return QStringLiteral("Modified Kullback-Liebler");
    case E::SpectralFlux: return QStringLiteral("Spectral Flux");
    }
}

} // namespace Audio::Aubio
