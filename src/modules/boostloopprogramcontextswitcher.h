#ifndef BOOSTLOOPPROGRAMCONTEXTSWITCHER_H
#define BOOSTLOOPPROGRAMCONTEXTSWITCHER_H

#include "loopprogram.hpp"
#include "boost/coroutine2/all.hpp"
#include <boost/config.hpp>

#if defined(BOOST_NO_CXX11_AUTO_DECLARATIONS) || defined(BOOST_NO_CXX11_CONSTEXPR) || defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || defined(BOOST_NO_CXX11_FINAL) ||                                  \
    defined(BOOST_NO_CXX11_HDR_TUPLE) || defined(BOOST_NO_CXX11_NOEXCEPT) || defined(BOOST_NO_CXX11_NULLPTR) || defined(BOOST_NO_CXX11_RVALUE_REFERENCES) ||                                           \
    defined(BOOST_NO_CXX11_TEMPLATE_ALIASES) || defined(BOOST_NO_CXX11_UNIFIED_INITIALISATION_SYNTAX) || defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(BOOST_NO_HDR_ATOMIC) ||                 \
    defined(BOOST_NO_HDR_TUPLE)
#error "execution_context is prevented to be included in this compiler";
#endif

namespace Modules {

class BoostLoopProgramContextSwitcher : public LoopProgramContextSwitcher {
    using Coro = boost::coroutines2::coroutine<void>;
    Coro::pull_type *m_coro = nullptr;
    Coro::push_type *m_yield = nullptr;

public:
    virtual void yield() override;
    virtual bool resume() override;
    virtual void start() override;
    ~BoostLoopProgramContextSwitcher() override;
};

} // namespace Modules

#endif // BOOSTLOOPPROGRAMCONTEXTSWITCHER_H
