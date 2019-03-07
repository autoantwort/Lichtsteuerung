#ifndef DMXCONSUMER_H
#define DMXCONSUMER_H

#include "consumer.hpp"
#include <vector>
#include "idbase.h"

namespace Modules {

    /**
     * @brief Der DMXConsumer macht es möglich, in Moduleprogrammen dmx channel anzusprechen. Diese Daten werden dann in driver.cpp hier abgeholt und vom alten treiber angezeigt
     */
    class DMXConsumer : public Modules::TypedConsumer<brightness_t>, public IDBase<DMXConsumer>
    {
        NumericProperty<int> startDMXChannel;
        bool running = false;
    public:
        /**
         * @brief fillWithDMXConsumer füllt den data array mit den dmx daten aus dem DMXConsumern
         * @param data der daten array der an die DMX Geräte geht
         * @param length Die länge des Arrays
         */
        static void fillWithDMXConsumer(unsigned char * data, size_t length);
    public:
        DMXConsumer();
        virtual void start()override;
        virtual void show() override;
        virtual void stop() override;
        virtual void doStep(time_diff_t)override{}
        virtual const char * getName()const override{return "DMXConsumer";}
    };

}
#endif // DMXCONSUMER_H
