#ifndef CAPTURE_WINDOWS_SOUNDOUTPUT_GLOBAL_H
#define CAPTURE_WINDOWS_SOUNDOUTPUT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CAPTURE_WINDOWS_SOUNDOUTPUT_LIBRARY)
#  define CAPTURE_WINDOWS_SOUNDOUTPUTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CAPTURE_WINDOWS_SOUNDOUTPUTSHARED_EXPORT Q_DECL_IMPORT
#endif

extern "C" CAPTURE_WINDOWS_SOUNDOUTPUTSHARED_EXPORT int captureAudio(void(*init)(int channels),void(*callback)(float * data, unsigned int numFramesAvailable, bool * done));

#endif // CAPTURE_WINDOWS_SOUNDOUTPUT_GLOBAL_H
