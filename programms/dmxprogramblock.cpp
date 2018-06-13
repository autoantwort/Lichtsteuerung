#include "dmxprogramblock.h"

namespace Modules {
DMXProgramBlock::DMXProgramBlock()
{
    prog = new DMXProgram();
    addProgramm(prog);
    consumer = std::make_shared<DMXConsumer>();
    prog->setOutputLength(512);
    consumer->setInputLength(512);
    detail::Connection c(consumer);
    c.addTarget(512,prog,0);
    addConsumer(std::move(c));
}
}


