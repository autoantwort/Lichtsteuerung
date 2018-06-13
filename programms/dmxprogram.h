#ifndef DMXPROGRAM_H
#define DMXPROGRAM_H

#include "programm.hpp"

namespace Modules {

class DMXProgram : public BrightnessProgramm
{
    time_diff_t time;
public:
    DMXProgram();
    virtual const char* getName()const override{return "DMXProgramm";}
    virtual void start()override{time = 0;}
    virtual ProgrammState doStep(time_diff_t)override;
    virtual int getProgrammLengthInMS()override {return INFINITE;}
};

}

#endif // DMXPROGRAM_H
