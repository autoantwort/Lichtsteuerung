#ifndef MODULES_PROGRAMM_H
#define MODULES_PROGRAMM_H

#include "types.h"
#include "property.h"
#include <vector>
#include <functional>

namespace Modules {


class Programm : public PropertyBase{
protected:
    void show(void *values){
        if(callback)
            callback(values);
    }
public:
    Programm(const ValueType valueType):valueType(valueType){}
    const ValueType valueType;
    std::function<void(void*)> callback;
    virtual int getProgrammLengthInMS() = 0;
    virtual void setOutputLength(int length)=0;
    virtual void start() = 0;
    virtual bool doStep(time_diff_t) = 0;
    virtual ~Programm(){}
};

template<typename value_type_t>
class ProgrammTemplate : public Programm{
protected:
    value_type_t *values = nullptr;
    int size;
    void show(){
        Programm::show(values);
    }
public:
    ProgrammTemplate(const ValueType valueType):Programm(valueType){}
    virtual void setOutputLength(int length){
        size = length;
        if(values)
            delete [] values;
        values = new value_type_t[size];
    }
    virtual ~ProgrammTemplate(){delete values;}
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
