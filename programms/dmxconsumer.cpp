#include "dmxconsumer.h"

namespace Modules {

    DMXConsumer::DMXConsumer()
    {

    }

    void DMXConsumer::setInputSize(unsigned int size){
        channel.resize(size,-1);
    }

    void DMXConsumer::start(){}

    void DMXConsumer::show(void * data){
        brightness_t * b = static_cast<brightness_t*>(data);
        for(auto i = channel.cbegin();i!=channel.cend();++i,++b){
            if(*i>0){
                //dmxData[*i] = *b;
                //TODO;
            }
        }
    }

}
