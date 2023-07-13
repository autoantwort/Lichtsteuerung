#ifndef DMXPROGRAMBLOCK_H
#define DMXPROGRAMBLOCK_H

#include "dmxprogram.h"
#include "programblock.h"

namespace Modules {

/**
 * @brief Ein DMXProgramBlock der von Controller ausgeführt werden kann, damit DMXProgramm ausgeführt werden können.
 */
class DMXProgramBlock : public ProgramBlock {
    std::shared_ptr<DMXProgram> prog;
    std::shared_ptr<Consumer> consumer;

public:
    /**
     * @brief isLoaded returns true, if a consumer is loaded so that the dmxouput can be shown
     * @return true if a consumer exists
     */
    bool isLoaded() { return consumer.operator bool(); }
    void setDMXConsumer(std::shared_ptr<Consumer> consumer);
    DMXProgramBlock(std::shared_ptr<Consumer> consumer);
    ~DMXProgramBlock() {}
};
} // namespace Modules

#endif // DMXPROGRAMBLOCK_H
