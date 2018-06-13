#ifndef TESTLOOPPROGRAMM_H
#define TESTLOOPPROGRAMM_H

#include "programms/loopprogramm.h"
#include "programms/programm.hpp"

#include <iostream>
#include <qdebug.h>
void test();

class TestLoopProgramm : public Modules::LoopProgramm<Modules::BrightnessProgramm>
{
protected:
    virtual void loopProgramm()override{
        for(unsigned i = 0; i< length;++i){
            values[i] = 255;

            wait(50);
        }
    }
public:
    TestLoopProgramm() = default;
    virtual const char * getName()const override{return "TestLoopProgramm";}
    virtual int getProgrammLengthInMS() override{return length*50;}
    virtual void start() override{}
    virtual ~TestLoopProgramm()override = default ;
};

#endif // TESTLOOPPROGRAMM_H
