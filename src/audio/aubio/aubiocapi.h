#ifndef AUBIOCAPI_H
#define AUBIOCAPI_H

#include <type_traits>

class QString;

namespace Audio::Aubio {

namespace C_API {
// hide the c api behind the C_API namespace
#include <aubio/aubio.h>
} // namespace C_API

/**
 * @brief The OnsetDetectionFunction enum holds all possible onset detection functions
 */
enum class OnsetDetectionFunction {
    EnergyBased,
    SpectralDifference,
    HighFrequencyContent,
    ComplexDomain,
    PhaseDeviation,
    KullbackLiebler,
    ModifiedKullbackLiebler,
    SpectralFlux,
    Last = SpectralFlux,
};

/**
 * @brief toOnsetDetectionFunction converts a int to an OnsetDetectionFunction. The function asserts that the int is a valid OnsetDetectionFunction
 * @param i The int that should be convertet to a OnsetDetectionFunction
 * @return the corresponding OnsetDetectionFunction (the function asserts if there is no corresponsing OnsetDetectionFunction)
 */
OnsetDetectionFunction toOnsetDetectionFunction(int i);

/**
 * Converts an enum to the underlying type (mostly a int)
 */
template <typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

namespace C_API {
    /**
     * @brief toName Converts the enum to an string that is used by the aubio c lib
     * @param f the enum that should be converted
     * @return the c string representation of the enum that can be used with the aubio lib
     */
    const C_API::char_t *toName(OnsetDetectionFunction f);
} // namespace C_API

/**
 * @brief toQString Converts the enum to a human readable string
 * @param f the enum that should be converted
 * @return a human readable string
 */
QString toQString(OnsetDetectionFunction f);

} // namespace Audio::Aubio

#endif // AUBIOCAPI_H
