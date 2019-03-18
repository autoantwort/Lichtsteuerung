#ifndef MODULES_PROGRAMM_H
#define MODULES_PROGRAMM_H

#include "types.h"
#include "property.hpp"
#include <vector>
#include <functional>

namespace Modules {


class AbstractProgramm{
public:
    /**
     * @brief doStep lässt das Programm einen Schritt machen
     * @return true wenn das programm fertig ist, sonst false
     */
    virtual bool doStep(time_diff_t) = 0;
    virtual ~AbstractProgramm() = default;
};

struct ProgramState{
    bool finished;
    bool outputDataChanged;
};

class Program : public PropertyBase, public OutputDataProducer, public Named{
public:
    static const int Infinite = (1 << 30);
    Program(const ValueType valueType):OutputDataProducer(valueType){}
    virtual int getProgrammLengthInMS() = 0;
    virtual void setOutputLength(unsigned int length)=0;
    virtual void start() = 0;
    virtual ProgramState doStep(time_diff_t) = 0;
    virtual ~Program() override = default;
    virtual void load(const LoadObject &l)override{
        PropertyBase::load(l);
        int length = l.loadInt("outputLength",0);
        if(length>0)
            setOutputLength(static_cast<unsigned>(length));
    }
    virtual void save(SaveObject &s) const override{
        PropertyBase::save(s);
        s.saveInt("outputLength",static_cast<int>(getOutputLength()));
    }
};

template<typename OutputType, typename Base = Program>
class TypedProgram : public Base{
    static_assert (std::is_base_of<Program,Base>::value,"The Base class must be an subclass of the Program class!");
protected:
    OutputType *output = nullptr;
    unsigned int outputLength = 0;
public:
    TypedProgram():Base(typeToEnum<OutputType>()){}
    virtual void setOutputLength(unsigned int l) override{
        outputLength = l;
        if(output)
            delete [] output;
        output = new OutputType[outputLength];
    }
    virtual unsigned int getOutputLength()const override{
        return outputLength;
    }
    virtual void* getOutputData()override{return output;}
    virtual ~TypedProgram()override{delete [] output;}
};



}

#endif // MODULES_PROGRAMM_H
