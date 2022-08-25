#ifndef DMXPROGRAM_H
#define DMXPROGRAM_H

#include "program.hpp"

namespace Modules {

class DMXProgram : public TypedProgram<brightness_t> {
    time_diff_t time;

public:
    DMXProgram();
    virtual const char *getName() const override { return "DMXProgramm"; }
    virtual void start() override { time = 0; }
    virtual ProgramState doStep(time_diff_t) override;
    virtual int getProgrammLengthInMS() override { return Infinite; }
};

} // namespace Modules

#endif // DMXPROGRAM_H
