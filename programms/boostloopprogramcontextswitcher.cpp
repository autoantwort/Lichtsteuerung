#include "boostloopprogramcontextswitcher.h"
#include <QDebug>

namespace Modules {


    BoostLoopProgramContextSwitcher::BoostLoopProgramContextSwitcher():m_coro([this](Coro::push_type &y){
        m_yield = &y;
        y();
        startLoopProgram();
    }){

    }

bool BoostLoopProgramContextSwitcher::resume(){
    m_coro();
    return !m_coro;
}

void BoostLoopProgramContextSwitcher::yield(){
    (*m_yield)();
}


}
