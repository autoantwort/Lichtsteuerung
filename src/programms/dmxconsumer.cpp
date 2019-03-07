#include "dmxconsumer.h"

namespace Modules {

    DMXConsumer::DMXConsumer():startDMXChannel(0,512,0)
    {
        properties.push_back(&startDMXChannel);
        startDMXChannel.setName("start DMX Channel");
        startDMXChannel.setDescription("The address of the first position in the output array.");
    }


    void DMXConsumer::start(){
        running = true;
    }
    void DMXConsumer::stop(){
        running = false;
    }

    void DMXConsumer::show(){
        // all the logic is done in fillWithDMXConsumer, because we are not the driver and the driver gets the data from us
    }

    void DMXConsumer::fillWithDMXConsumer(unsigned char *data, size_t length){
        for(const auto & i : IDBase<DMXConsumer>::getAllIDBases()){
            if(i->running){
                for (int a = 0; a < std::min(static_cast<int>(length)-i->startDMXChannel.getValue(),static_cast<int>(i->getInputLength()));++a) {
                    data[i->startDMXChannel.getValue() + a] = i->input[a];
                }
            }
        }
    }

}
