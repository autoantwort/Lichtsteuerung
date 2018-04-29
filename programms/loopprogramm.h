#ifndef LOOPPROGRAMM_H
#define LOOPPROGRAMM_H

#include "programm.hpp"
#include "types.h"
#include "boost/coroutine2/all.hpp"
#include <qdebug.h>

#include <boost/config.hpp>

#if defined(BOOST_NO_CXX11_AUTO_DECLARATIONS) || \
     defined(BOOST_NO_CXX11_CONSTEXPR) || \
     defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || \
     defined(BOOST_NO_CXX11_FINAL) || \
     defined(BOOST_NO_CXX11_HDR_TUPLE) || \
     defined(BOOST_NO_CXX11_NOEXCEPT) || \
     defined(BOOST_NO_CXX11_NULLPTR) || \
     defined(BOOST_NO_CXX11_RVALUE_REFERENCES) || \
     defined(BOOST_NO_CXX11_TEMPLATE_ALIASES) || \
     defined(BOOST_NO_CXX11_UNIFIED_INITIALISATION_SYNTAX) || \
     defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || \
     defined(BOOST_NO_HDR_ATOMIC) || \
     defined(BOOST_NO_HDR_TUPLE)
#error "execution_context is prevented to be included in this compiler";
#endif
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
    using Coro = boost::coroutines2::coroutine<void>;
    Coro::pull_type coro;
    Coro::push_type *yield;

    bool first=true;
    bool finished = false;
    bool show_=false;
    time_diff_t currentWaitTime = 0;
    time_diff_t timeToWait = 0;
    static_assert (std::is_base_of<Programm,BaseClass>::value,"BaseClass must be an Subclass of Programm or the clas Programm.");
protected:
    void wait(time_diff_t time){
        currentWaitTime = 0;
        timeToWait = time;
        (*yield)();
    }
    virtual void loopProgramm() = 0;
public:
    LoopProgramm():coro([this](Coro::push_type &y){
        yield = &y;
        y();
        loopProgramm();
        finished = true;}){}
    virtual ~LoopProgramm(){}
    virtual ProgrammState doStep(time_diff_t t)override{
        if(first){
            first = false;
        }
        if(finished)
            return {true,false};
        currentWaitTime+=t;
        if(currentWaitTime>=timeToWait){
            currentWaitTime = 0;
            coro();
            return {finished,true};
        }
        return {false,false};
    }
};

}

#endif // LOOPPROGRAMM_H
