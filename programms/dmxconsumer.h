#ifndef DMXCONSUMER_H
#define DMXCONSUMER_H

#include "consumer.h"
#include <vector>

namespace Modules {

    class DMXConsumer : public Modules::Consumer
    {
        std::vector<short> channel;
    public:
        DMXConsumer();
        virtual ValueType getInputValueType()override{return Brightness;}
        virtual void setInputSize(unsigned int) override;
        virtual void start()override;
        virtual void show(void * data) override;
        const std::vector<short> & getChannelAssoziatfion()const{return channel;}
    };

}
#endif // DMXCONSUMER_H
