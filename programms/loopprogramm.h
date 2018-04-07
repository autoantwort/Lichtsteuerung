#ifndef LOOPPROGRAMM_H
#define LOOPPROGRAMM_H

#include "programm.h"
#include <csetjmp>
#include "types.h"

namespace Modules {

template<typename BaseClass>
class LoopProgramm : public BaseClass
{
    /**
     *  Address : 0 ... 100 101 102 103 ... 1000
     *  eg:              ^           ^
     *                   |           + Stackbuttom
     *                   + Stacktop
     * Stacktop have smaller address then stackbuttom.
     */

    unsigned char * stack = nullptr;
    int stackSize = 0;
    int maxStackSize = 0;
    void * stackButtom;
    void * stackTop;
    bool first = true;
    bool finished = false;
    jmp_buf waitPos;
    jmp_buf doStepPos;
    time_diff_t currentWaitTime = 0;
    time_diff_t timeToWait = 0;
    static_assert (std::is_base_of<Programm,BaseClass>::value,"BaseClass must be an Subclass of Programm or the clas Programm.");
protected:
    void wait(time_diff_t time){
        currentWaitTime = 0;
        timeToWait = time;
        if(!setjmp(waitPos)){
            volatile int currentStackSize;
            stackTop = reinterpret_cast<void*>(reinterpret_cast<size_t>(&currentStackSize));
            currentStackSize = size_t(stackButtom)-size_t(stackTop);
            if(currentStackSize>maxStackSize){
                delete [] stack;
                stack = new unsigned char[maxStackSize];
                maxStackSize = currentStackSize;
            }
            stackSize = currentStackSize;
            std::memcpy(stack,stackTop,stackSize);
            longjmp(doStepPos,1);
        }else{
            // restore stack
            std::memcpy(reinterpret_cast<void*>((size_t(stackButtom)-stackSize)),stack,stackSize);
        }
    }
    virtual void loopProgramm() = 0;
public:
    LoopProgramm() = default;
    virtual bool doStep(time_diff_t t)override{
        if(finished)
            return true;
        currentWaitTime+=t;
        if(currentWaitTime>=timeToWait){
            currentWaitTime = 0;
            volatile char stack_bottom;
            if(!setjmp(doStepPos)){
                // button for savong stack
                stackButtom = (void*)(size_t)&stack_bottom;
                if(first){
                    first = false;
                    loopProgramm();
                    finished = true;
                }else{
                    // jump into the wait function
                    longjmp(waitPos,1);
                }
            }
        }
        return false;
    }
};

}

#endif // LOOPPROGRAMM_H
