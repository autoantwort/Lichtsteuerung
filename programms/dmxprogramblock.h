#ifndef DMXPROGRAMBLOCK_H
#define DMXPROGRAMBLOCK_H

#include "programblock.h"
#include "dmxprogram.h"
#include "dmxconsumer.h"

namespace Modules {
    class DMXProgramBlock : public ProgramBlock
    {
        DMXProgram * prog = nullptr;
        std::shared_ptr<DMXConsumer> consumer;
    public:
        DMXProgramBlock();
        ~DMXProgramBlock(){}
    };
}


#endif // DMXPROGRAMBLOCK_H
