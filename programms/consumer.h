#ifndef COMSUMER_H
#define COMSUMER_H

#include "types.h"

class Consumer{

public:
    virtual ValueType getInputValueType()=0;
    virtual void setInputSize(unsigned int)=0;
    virtual void start()=0;
    virtual void show(void * data) = 0;
    virtual ~Consumer() = default;
};

#endif // COMSUMER_H
