#include "dmxprogram.h"
#include "dmxchannelfilter.h"
#include "programm.h"
#include <cstring>

namespace Modules {

    DMXProgram::DMXProgram()
    {

    }

    ProgrammState DMXProgram::doStep(time_diff_t diff) {
        time += diff;
        std::memset(values,0,getOutputLength());
        DMXChannelFilter::initValues(values,getOutputLength());
        ::Programm::fill(values,getOutputLength(),time/1000.);
        DMXChannelFilter::filterValues(values,getOutputLength());
        return {false,true};
    }
}
