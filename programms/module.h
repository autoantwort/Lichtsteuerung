#ifndef MODULE_GLOBAL_H
#define MODULE_GLOBAL_H

// define one of the following if your module have the type
//#define HAVE_PROGRAM
//#define HAVE_LOOP_PROGRAMM
//#define HAVE_FILTER
//#define HAVE_CONSUMER

#ifdef HAVE_PROGRAM
#include "programm.hpp"
#endif

#ifdef HAVE_FILTER
#include "filter.hpp"
#endif

#ifdef HAVE_CONSUMER
#include "consumer.hpp"
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
enum MODUL_TYPE{Programm, LoopProgramm,Filter,Consumer};

MODULE_EXPORT bool have(MODUL_TYPE t){
    switch (t) {
    case Programm:
#ifdef HAVE_PROGRAM
    return true;
#else
    return false;
#endif
    case LoopProgramm:
#ifdef HAVE_LOOP_PROGRAMM
    return true;
#else
    return false;
#endif
    case Filter:
#ifdef HAVE_FILTER
    return true;
#else
    return false;
#endif
    case Consumer:
#ifdef HAVE_CONSUMER
    return true;
#else
    return false;
#endif
    }
    return false; // Vielleicht wird das enum in einer weiteren Version erweitert.
}

#ifdef HAVE_PROGRAM
MODULE_EXPORT unsigned int getNumberOfProgramms();
MODULE_EXPORT char const * getNameOfProgramm(unsigned int index);
MODULE_EXPORT Modules::Programm * createProgramm(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfProgramm(unsigned int index);
#endif

#ifdef HAVE_FILTER
MODULE_EXPORT unsigned int getNumberOfFilter();
MODULE_EXPORT char const * getNameOfFilter(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfFilter(unsigned int index);
MODULE_EXPORT Modules::Filter * createFilter(unsigned int index);
#endif

#ifdef HAVE_CONSUMER
MODULE_EXPORT unsigned int getNumberOfComsumer();
MODULE_EXPORT char const * getNameOfConsumer(unsigned int index);
MODULE_EXPORT char const * getDescriptionOfConsumer(unsigned int index);
MODULE_EXPORT Modules::Consumer * createConsumer(unsigned int index);
#endif

}

#endif // MODULES_GLOBAL_H
