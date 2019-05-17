#ifndef TESTPROGRAMMBLOCK_H
#define TESTPROGRAMMBLOCK_H

#include "modules/controller.h"
#include "modules/program.hpp"
#include "modules/consumer.hpp"
#include <cstring>

namespace  Test{

class TestProgramm : public Modules::TypedProgram<Modules::brightness_t>{
int i=0;
public:
    int getProgrammLengthInMS(){return outputLength;}
    void start(){
        std::memset(output,0,sizeof (Modules::brightness_t) * outputLength);
    }
    Modules::ProgramState doStep(Modules::time_diff_t){
        output[i] = 255;
        ++i;
        if(i>=static_cast<int>(outputLength)){
            return {true,true};
        }
        return {false,true};
    }

    virtual const char* getName()const {return "TestProgramm";}
};

class TestFilter : public Modules::TypedFilter<Modules::brightness_t,Modules::brightness_t>{
public:

void filter()override{
    for (unsigned i = 0 ; i <inputLength;++i) {
        output[i] = input[i]/2;
    }
}

    bool doStep(Modules::time_diff_t){
        return false;
    }

    virtual const char* getName()const {return "TestFilter";}
    virtual unsigned int computeOutputLength(unsigned int inputLength) {return inputLength;}
};

class ControlConsumer : public Modules::TypedConsumer<Modules::brightness_t>{
    unsigned index = 0;
public:
virtual void start(){}
virtual void stop(){}
    /**
     * @brief show zeigt die eingabedaten an
     */
    virtual void show(){
        ++index;
        for (unsigned i = 0;i<index;++i) {
            Q_ASSERT(input[i]==255/2);
        }
        for (unsigned i = index;i<inputLength;++i) {
            Q_ASSERT(input[i]==0);
        }
    }
    virtual void doStep(Modules::time_diff_t diff){
        Q_UNUSED(diff)
        for (unsigned i = 0;i<index;++i) {
            Q_ASSERT(input[i]==255/2);
        }
        for (unsigned i = index;i<inputLength;++i) {
            Q_ASSERT(input[i]==0);
        }
    }

    virtual const char* getName()const {return "ControlConsumer";}
};

class TestProgrammBlock
{
public:
    TestProgrammBlock();
    void test();
};

}

#endif // TESTPROGRAMMBLOCK_H
