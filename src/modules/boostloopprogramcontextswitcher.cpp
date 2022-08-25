#include "boostloopprogramcontextswitcher.h"
#include <QDebug>

namespace Modules {

bool BoostLoopProgramContextSwitcher::resume() {
    if (m_coro) {
        (*m_coro)();
        return !(*m_coro);
    }
    return true;
}

void BoostLoopProgramContextSwitcher::yield() {
    if (m_yield) (*m_yield)();
}

void BoostLoopProgramContextSwitcher::start() {
    if (m_coro) {
        delete m_coro;
        m_coro = nullptr;
        m_yield = nullptr;
    }
    m_coro = new Coro::pull_type([this](Coro::push_type &y) {
        m_yield = &y;
        y();
        startLoopProgram();
    });
}

BoostLoopProgramContextSwitcher::~BoostLoopProgramContextSwitcher() {
    if (m_coro) delete m_coro;
}

} // namespace Modules
