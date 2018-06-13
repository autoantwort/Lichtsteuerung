#ifndef COMSUMER_H
#define COMSUMER_H

#include "types.h"
#include <cstring>

namespace Modules{

    /**
     * @brief The Consumer class represents a Consumer like a driver for dmx or leds
     * Treiber erben von dieser Klasse und zeigen die Entsprechenden Daten an.
     */
    class Consumer:public PropertyBase, public InputDataConsumer, public Named{
    public:
        Consumer(const ValueType inputDataType):InputDataConsumer(inputDataType){}
        virtual void setInputLength(unsigned int)=0;
        virtual void start()=0;
        /**
         * @brief show zeigt die eingabedaten an
         */
        virtual void show() = 0;
        virtual void doStep(time_diff_t diff)=0;
        virtual ~Consumer()override = default;
        virtual void load(const LoadObject &l)override{
            PropertyBase::load(l);
            int length = l.loadInt("inputLength");
            if(length>0)
                setInputLength(static_cast<unsigned>(length));
        }
        virtual void save(SaveObject &s) const override{
            PropertyBase::save(s);
            s.saveInt("inputLength",static_cast<int>(getInputLength()));
        }
    };

    template<typename Type>
    class TypedConsumer : public Consumer{
    protected:
        Type * input = nullptr;
        unsigned int inputLength = 0;
    public:
        TypedConsumer():Consumer(typeToEnum<Type>()){}
        virtual void setInputData(void * data, unsigned int index, unsigned int length)override{
            if(index<inputLength){
                const auto l = index+length>inputLength?inputLength-index:length;
                std::memcpy(input+index,data,l*sizeof (Type));
            }
        }
        virtual void setInputLength(unsigned int length)override{
            if(inputLength!=length){
                delete [] input;
                inputLength = length;
                input = new Type[inputLength];
            }
        }
        virtual unsigned int getInputLength()const override final{return inputLength;}
    };
}
#endif // COMSUMER_H
