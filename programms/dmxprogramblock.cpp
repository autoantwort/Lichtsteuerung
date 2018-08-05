#include "dmxprogramblock.h"

namespace Modules {
DMXProgramBlock::DMXProgramBlock(std::shared_ptr<Consumer> consumer)
{
    prog = std::make_shared<DMXProgram>();
    addProgramm(prog);
    this->consumer = consumer;
    prog->setOutputLength(512);
    consumer->setInputLength(512);
    detail::Connection c(consumer);
    c.addTarget(512,prog.get(),0);
    addConsumer(std::move(c));
}

void DMXProgramBlock::setDMXConsumer(std::shared_ptr<Consumer> consumer){
    this->consumer = consumer;
    consumer->setInputLength(512);
    removeConsumer([](const detail::Connection&){return true;});
    detail::Connection c(consumer);
    c.addTarget(512,prog.get(),0);
    addConsumer(std::move(c));
}

}


