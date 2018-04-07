#ifndef TESTLOOPPROGRAMM_H
#define TESTLOOPPROGRAMM_H

#include "programms/loopprogramm.h"
#include "programms/programm.h"

#include <iostream>
void test();

class TestLoopProgramm : public Modules::LoopProgramm<Modules::BrightnessProgramm>
{
protected:
    virtual void loopProgramm()override{
        for(int i = 0; i< size;++i){
            values[i] = 255;
            show();
            std::cout << i<<std::endl;
            wait(50);
        }
        std::cout<<"finished\n";
    }
public:
    TestLoopProgramm() = default;

    virtual int getProgrammLengthInMS() override{return size*50;}
    virtual void start() override{}
    virtual ~TestLoopProgramm()override = default ;
};

#endif // TESTLOOPPROGRAMM_H
