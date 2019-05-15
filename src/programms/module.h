#ifndef MODULE_GLOBAL_H
#define MODULE_GLOBAL_H

// define one of the following if your module have the type
//#define HAVE_PROGRAM
//#define HAVE_LOOP_PROGRAMM
//#define HAVE_FILTER
//#define HAVE_CONSUMER
//#define HAVE_SPOTIFY
//#define HAVE_CONTROL_POINT

#ifdef HAVE_PROGRAM
#include "program.hpp"
#endif

#ifdef HAVE_LOOP_PROGRAM
#include "loopprogram.hpp"
#endif

#ifdef HAVE_FILTER
#include "filter.hpp"
#endif

#ifdef HAVE_CONSUMER
#include "consumer.hpp"
#endif

#ifdef HAVE_AUDIO
#include "fftoutput.hpp"
#endif

#ifdef HAVE_SPOTIFY
#undef CONVERT_FROM_SPOTIFY_OBJECTS
#include "spotify.hpp"
#endif

#ifdef HAVE_CONTROL_POINT
#include "controlpoint.hpp"
#endif

#include <string>

//disable Warning for char * as return type in extern "C" Block with clang
//#pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"

#if defined(__APPLE__) || defined(__linux__)
    #define MODULE_EXPORT
#else
    #if defined(MODULE_LIBRARY)
    #  define MODULE_EXPORT __declspec(dllexport)
    #else
    #  define MODULE_EXPORT __declspec(dllimport)
    #endif
#endif

extern "C" {
enum class MODUL_TYPE{Program, LoopProgram,Filter,Consumer,Audio,Spotify,ControlPoint};

#ifdef MODULE_LIBRARY

MODULE_EXPORT bool have(MODUL_TYPE t){
    switch (t) {
    case MODUL_TYPE::Program:
#ifdef HAVE_PROGRAM
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::LoopProgram:
#ifdef HAVE_LOOP_PROGRAM
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::Filter:
#ifdef HAVE_FILTER
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::Consumer:
#ifdef HAVE_CONSUMER
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::Audio:
#ifdef HAVE_AUDIO
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::Spotify:
#ifdef HAVE_SPOTIFY
    return true;
#else
    return false;
#endif
    case MODUL_TYPE::ControlPoint:
#ifdef HAVE_CONTROL_POINT
    return true;
#else
    return false;
#endif
    }
    return false; // Vielleicht wird das enum in einer weiteren Version erweitert.
}

#endif

#ifdef HAVE_PROGRAM
MODULE_EXPORT unsigned int getNumberOfPrograms();
MODULE_EXPORT char const * getNameOfProgram(unsigned int index);
MODULE_EXPORT Modules::Program * createProgram(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfProgram(unsigned int index);
#endif

#ifdef HAVE_LOOP_PROGRAM
MODULE_EXPORT unsigned int getNumberOfLoopPrograms();
MODULE_EXPORT char const * getNameOfLoopProgram(unsigned int index);
MODULE_EXPORT Modules::LoopProgram * createLoopProgram(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfLoopProgram(unsigned int index);
#endif

#ifdef HAVE_FILTER
MODULE_EXPORT unsigned int getNumberOfFilters();
MODULE_EXPORT char const * getNameOfFilter(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfFilter(unsigned int index);
MODULE_EXPORT Modules::Filter * createFilter(unsigned int index);
#endif

#ifdef HAVE_CONSUMER
MODULE_EXPORT unsigned int getNumberOfConsumers();
MODULE_EXPORT char const * getNameOfConsumer(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfConsumer(unsigned int index);
MODULE_EXPORT Modules::Consumer * createConsumer(unsigned int index);
#endif

#ifdef HAVE_AUDIO
Modules::FFTOutputView<float> _emptyView;
Modules::FFTOutputView<float> & fftOutput = _emptyView;
bool __supportAudio = false;
MODULE_EXPORT void _supportAudio(bool b){__supportAudio = b;}
MODULE_EXPORT void _setFFTOutputView(Modules::FFTOutputView<float> * fftOutputView){fftOutput = fftOutputView?*fftOutputView:_emptyView;}
bool supportAudio(){return __supportAudio;}
#endif

#ifdef HAVE_SPOTIFY
Modules::SpotifyState __emptySpotifyState;
Modules::SpotifyState const * spotify = &__emptySpotifyState;
MODULE_EXPORT void _setSpotifyState(Modules::SpotifyState const * s){spotify = s?s:&__emptySpotifyState;}
#endif

#ifdef HAVE_CONTROL_POINT
Modules::ControlPoint __defaultControlPoint;
Modules::ControlPoint const * controlPoint;
MODULE_EXPORT void _setControlPoint(Modules::ControlPoint const * s){controlPoint = s?s:&__defaultControlPoint;}
#endif

}

#endif // MODULES_GLOBAL_H
