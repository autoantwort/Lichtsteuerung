#ifndef FILTER_H
#define FILTER_H

#include "types.h"

namespace Modules {

    class Filter : public PropertyBase{
    public:
        void setInputLength(int length);
        int getOutputLength();
        ValueType getInputType();
        ValueType getOutputType();
        void filter(void * inputData, void * outputData);
    };
}

#endif // FILTER_H
