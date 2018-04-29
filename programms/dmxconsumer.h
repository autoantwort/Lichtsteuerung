#ifndef DMXCONSUMER_H
#define DMXCONSUMER_H

#include "consumer.hpp"
#include <vector>

namespace Modules {

    class DMXConsumer : public Modules::TypedConsumer<brightness_t>
    {
        std::vector<short> channel;
    public:
        DMXConsumer();
        virtual void setInputLength(unsigned int) override;
        virtual void start()override;
        virtual void show() override;
        const std::vector<short> & getChannelAssoziatfion()const{return channel;}
    };

}
#endif // DMXCONSUMER_H
