#ifndef AUDIO_FFT_GLOBAL_H
#define AUDIO_FFT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AUDIO_FFT_LIBRARY)
#  define AUDIO_FFT_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define AUDIO_FFT_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AUDIO_FFT_GLOBAL_H
