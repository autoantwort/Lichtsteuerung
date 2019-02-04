#include "dmxconsumer.h"

namespace Modules {

    DMXConsumer::DMXConsumer()
    {

    }

    void DMXConsumer::setInputLength(unsigned int size){
        channel.resize(size,-1);
        TypedConsumer::setInputLength(size);
    }

    void DMXConsumer::start(){}

    void DMXConsumer::show(){
        brightness_t * b = input;
        for(auto i = channel.cbegin();i!=channel.cend();++i,++b){
            if(*i>0){
                //dmxData[*i] = *b;
                //TODO;
            }
        }
    }

}
