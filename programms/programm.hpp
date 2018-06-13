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

class Programm : public PropertyBase, public OutputDataProducer, public Named{
public:    
    static const int INFINITE = 1 << 30;
    Programm(const ValueType valueType):OutputDataProducer(valueType){}
    virtual int getProgrammLengthInMS() = 0;
    virtual void setOutputLength(unsigned int length)=0;
    virtual void start() = 0;
    virtual ProgrammState doStep(time_diff_t) = 0;
    virtual ~Programm() override = default;
    virtual void load(const LoadObject &l)override{
        PropertyBase::load(l);
        int length = l.loadInt("outputLength");
        if(length>0)
            setOutputLength(static_cast<unsigned>(length));
    }
    virtual void save(SaveObject &s) const override{
        PropertyBase::save(s);
        s.saveInt("outputLength",static_cast<int>(getOutputLength()));
    }
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
