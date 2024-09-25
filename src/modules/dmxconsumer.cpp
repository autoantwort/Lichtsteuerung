#include "dmxconsumer.h"

namespace Modules {

/**
 * @brief consumers a list of all existing dmxconsumers
 */
std::vector<DMXConsumer *> consumers;

DMXConsumer::DMXConsumer() : startDMXChannel(0, 512, 0) {
    properties.push_back(&startDMXChannel);
    startDMXChannel.setName("start DMX Channel");
    startDMXChannel.setDescription("The address of the first position in the output array.");
    consumers.push_back(this);
}

DMXConsumer::~DMXConsumer() {
    for (auto i = consumers.cbegin(); i != consumers.cend(); ++i) {
        if (*i == this) {
            consumers.erase(i);
            return;
        }
    }
}

void DMXConsumer::start() {
    running = true;
}
void DMXConsumer::stop() {
    running = false;
}

void DMXConsumer::show() {
    // all the logic is done in fillWithDMXConsumer, because we are not the driver and the driver gets the data from us
}

void DMXConsumer::fillWithDMXConsumer(unsigned char *data, size_t length) {
    for (const auto &i : consumers) {
        const auto index = i->startDMXChannel.getValue() - 1 /* address 1 base, array 0 based index */;
        if (i->running && index < static_cast<int>(length)) {
            const auto numElements = std::min(static_cast<int>(length) - index, static_cast<int>(i->getInputLength()));
            std::copy_n(i->input, numElements, data + index);
        }
    }
}

} // namespace Modules
