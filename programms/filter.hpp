#ifndef FILTER_H
#define FILTER_H

#include "types.h"
#include <cstring>

namespace Modules {

/**
     * @brief The Filter class filters input data and write the filtered data in the outpur array
     */
    class Filter : public PropertyBase, public OutputDataProducer, public InputDataConsumer, public Named{
    public:
        Filter(const ValueType inputDataType,const ValueType outputDataType):OutputDataProducer(outputDataType),InputDataConsumer(inputDataType){}
        virtual void setInputLength(unsigned int length) = 0;
        /**
         * @brief filter override this method to filter the input data and write it to the output data
         */
        virtual void filter() = 0;
        /**
         * @brief doStep you can modify the output data to create spezial effects like fade outs
         * @return true if the output data changed
         */
        virtual bool doStep(time_diff_t){return false;}
        virtual ~Filter() = default;
    };

    template<typename InputType,typename OutputType>
    class TypedFilter : public Filter{
    protected:
        InputType * input = nullptr;
        OutputType * output = nullptr;
        unsigned int inputLength = 0;
        unsigned int outputLength = 0;
    protected:
        virtual unsigned int computeOutputLength(unsigned int inputLength) = 0;
    public:
        TypedFilter():Filter(typeToEnum<InputType>(),typeToEnum<OutputType>()){}
        virtual unsigned int getInputLength()const final  override{return inputLength;}
        virtual unsigned int getOutputLength()const final override{return outputLength;}
        virtual void * getOutputData()final override{return output;}
        virtual void setInputData(void*data, unsigned int index, unsigned int length)final override{
            if(index<inputLength){
                const auto l = index+length>inputLength?inputLength-index:length;
                std::memcpy(input+index,data,l * sizeof (InputType));
            }
        }
        virtual void setInputLength(unsigned length)final override{
            if(inputLength!=length){
                inputLength = length;
                delete [] input;
                input = new InputType[inputLength];
                auto newOutputLength = computeOutputLength(inputLength);
                if(newOutputLength!=outputLength){
                    delete [] output;
                    outputLength = newOutputLength;
                    output = new OutputType[outputLength];
                }
            }
        }
    };
}

#endif // FILTER_H
