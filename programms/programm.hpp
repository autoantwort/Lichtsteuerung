#ifndef MODULES_PROGRAMM_H
#define MODULES_PROGRAMM_H

#include "types.h"
#include "property.hpp"
#include <vector>
#include <functional>

namespace Modules {


class AbstractProgramm{
public:
    virtual bool doStep(time_diff_t) = 0;
    virtual ~AbstractProgramm() = default;
};

struct ProgrammState{

    bool finished;
    bool outputDataChanged;
};

class Programm : public PropertyBase, public OutputDataProducer{
public:    
    Programm(const ValueType valueType):OutputDataProducer(valueType){}
    virtual int getProgrammLengthInMS() = 0;
    virtual void setOutputLength(unsigned int length)=0;
    virtual void start() = 0;
    virtual ProgrammState doStep(time_diff_t) = 0;
    virtual ~Programm(){}
};

template<typename value_type_t>
class ProgrammTemplate : public Programm{
protected:
    value_type_t *values = nullptr;
    unsigned int length = 0;
public:
    ProgrammTemplate(const ValueType valueType):Programm(valueType){}
    virtual void setOutputLength(unsigned int l) override{
        length = l;
        if(values)
            delete [] values;
        values = new value_type_t[length];
    }
    virtual unsigned int getOutputLength()const override{
        return length;
    }
    virtual void* getOutputData()override{return values;}
    virtual ~ProgrammTemplate()override{delete [] values;}
};

class RGBProgramm:public ProgrammTemplate<rgb_t>{
public:
    RGBProgramm():ProgrammTemplate(RGB){}
};

class BrightnessProgramm: public ProgrammTemplate<brightness_t>{
public:
    BrightnessProgramm():ProgrammTemplate(Brightness){}
};

}

#endif // MODULES_PROGRAMM_H
