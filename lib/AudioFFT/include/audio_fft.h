#ifndef AUDIO_FFT_H
#define AUDIO_FFT_H

#include "audio_fft_global.h"
#include "fftw3.h"

class AUDIO_FFT_SHARED_EXPORT AudioFFT
{
    fftw_plan plan;
    fftw_complex *in, *out;
    const unsigned int size;
public:
    AudioFFT(const unsigned int size);
    void execute();
    void analyse(float* floatData,const int stepsToNext, float*output);
    void fillInputData(float* floatData,const int stepsToNext);
    void fillInputDataAndApplyHannWindow(float* floatData,const int stepsToNext);
    fftw_complex * getInput(){return in;}
    fftw_complex * getOutput(){return out;}
    /**
     * @brief fillOutput füllt size/2 mem positionen mit dem Ergebnis der Analyse in log db scale
     * @param mem Ein size/2 langer float array der mit dem Ergebnis gefüllt wird
     */
    void fillOutput(float * mem);
    unsigned int getSize(){return size;}
    ~AudioFFT();
private:
    void createPlan();
    inline float hann(int n);
};



#endif // AUDIO_FFT_H
