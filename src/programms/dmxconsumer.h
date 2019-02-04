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
        virtual void doStep(time_diff_t)override{}
        const std::vector<short> & getChannelAssoziatfion()const{return channel;}
        virtual const char * getName()const override{return "DMXConsumer";}
    };

}
#endif // DMXCONSUMER_H
