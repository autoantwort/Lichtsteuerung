#ifndef MODULE_GLOBAL_H
#define MODULE_GLOBAL_H

// define one of the following if your module have the type
#define HAVE_PROGRAM
#define HAVE_LOOP_PROGRAMM
#define HAVE_FILTER
#define HAVE_CONSUMER

#ifdef HAVE_PROGRAM
#include "programm.h"
#endif

#ifdef HAVE_FILTER
#include "filter.h"
#endif

#ifdef HAVE_CONSUMER
#include "consumer.h"
#endif

#include <string>

#define MODULE_LIBRARY

#if defined(MODULE_LIBRARY)
#  define MODULE_EXPORT __declspec(dllexport)
#else
#  define MODULE_EXPORT __declspec(dllimport)
#endif

enum MODUL_TYPE{Programm, LoopProgramm,Filter,Consumer};
inline MODULE_EXPORT bool have(MODUL_TYPE t){
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
}

#ifdef HAVE_PROGRAM
MODULE_EXPORT unsigned int getNumberOfProgramms();
MODULE_EXPORT std::string getNameOfProgramm(unsigned int index);
MODULE_EXPORT Modules::Programm * createProgramm(unsigned int index);
MODULE_EXPORT std::string getDescriptionOfProgramm(unsigned int index);
#endif

#ifdef HAVE_FILTER
MODULE_EXPORT unsigned int getNumberOfFilter();
MODULE_EXPORT std::string getNameOfFilter(unsigned int index);
MODULE_EXPORT std::string getDescriptionOfFilter(unsigned int index);
MODULE_EXPORT Modules::Filter * createFilter(unsigned int index);
#endif

#ifdef HAVE_CONSUMER
MODULE_EXPORT unsigned int getNumberOfComsumer();
MODULE_EXPORT std::string getNameOfConsumer(unsigned int index);
MODULE_EXPORT std::string getDescriptionOfConsumer(unsigned int index);
MODULE_EXPORT Modules::Consumer * createConsumer(unsigned int index);
#endif

#endif // MODULES_GLOBAL_H
