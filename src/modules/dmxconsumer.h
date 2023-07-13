#ifndef DMXCONSUMER_H
#define DMXCONSUMER_H

#include "consumer.hpp"
#include "idbase.h"
#include <vector>

namespace Modules {

/**
 * @brief Der DMXConsumer macht es möglich, in Moduleprogrammen dmx channel anzusprechen. Diese Daten werden dann in driver.cpp hier abgeholt und vom alten treiber angezeigt
 */
class DMXConsumer : public Modules::TypedConsumer<brightness_t> {
    NumericProperty<int> startDMXChannel;
    bool running = false;

public:
    /**
     * @brief fillWithDMXConsumer füllt den data array mit den dmx daten aus dem DMXConsumern
     * @param data der daten array der an die DMX Geräte geht
     * @param length Die länge des Arrays
     */
    static void fillWithDMXConsumer(unsigned char *data, size_t length);

public:
    DMXConsumer();
    ~DMXConsumer() override;
    void start() override;
    void show() override;
    void stop() override;
    void doStep(time_diff_t /*diff*/) override {}
    const char *getName() const override { return "DMXConsumer"; }
};

} // namespace Modules
#endif // DMXCONSUMER_H
