#ifndef LOOPPROGRAMM_H
#define LOOPPROGRAMM_H

#include "programm.h"
#include "types.h"
#include "coroutine.h"

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
    coroutine::routine_t coro = coroutine::create([this](){
        loopProgramm();
        finished = true;
    });
    bool finished = false;
    time_diff_t currentWaitTime = 0;
    time_diff_t timeToWait = 0;
    static_assert (std::is_base_of<Programm,BaseClass>::value,"BaseClass must be an Subclass of Programm or the clas Programm.");
protected:
    void wait(time_diff_t time){
        currentWaitTime = 0;
        timeToWait = time;
        coroutine::yield();
    }
    virtual void loopProgramm() = 0;
public:
    LoopProgramm() = default;
    virtual ~LoopProgramm(){coroutine::destroy(coro);}
    virtual bool doStep(time_diff_t t)override{
        if(finished)
            return true;
        currentWaitTime+=t;
        if(currentWaitTime>=timeToWait){
            currentWaitTime = 0;
            coroutine::resume(coro);
            return finished;
        }
        return false;
    }
};

}

#endif // LOOPPROGRAMM_H
