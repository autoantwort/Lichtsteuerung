#include "dmxprogram.h"
#include "dmxchannelfilter.h"
#include "programm.h"
#include <cstring>

namespace Modules {

    DMXProgram::DMXProgram()
    {

    }

    ProgramState DMXProgram::doStep(time_diff_t diff) {
        time += diff;
        std::memset(output,0,getOutputLength());
        DMXChannelFilter::initValues(output,getOutputLength());
        ::Programm::fill(output,getOutputLength(),time/1000.);
        DMXChannelFilter::filterValues(output,getOutputLength());
        return {false,true};
    }
}
