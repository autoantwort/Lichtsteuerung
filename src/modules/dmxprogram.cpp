#include "dmxprogram.h"
#include "dmx/dmxchannelfilter.h"
#include "dmx/programm.h"
#include <cstring>

namespace Modules {

    DMXProgram::DMXProgram()
    {

    }

    ProgramState DMXProgram::doStep(time_diff_t diff) {
        time += diff;
        std::memset(output,0,getOutputLength());
        DMX::DMXChannelFilter::initValues(output,getOutputLength());
        DMX::Programm::fill(output,getOutputLength(),time/1000.);
        DMX::DMXChannelFilter::filterValues(output,getOutputLength());
        return {false,true};
    }
}
