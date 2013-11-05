#ifndef BLOCK_SIGNALS_H
#define BLOCK_SIGNALS_H

#include <signal.h>

#include <initializer_list>

namespace sysapi
{
    sigset_t make_sigset(std::initializer_list<int> const&);
    sigset_t make_sigset(int);

    struct block_signals
    {
        block_signals(const sigset_t&);
        ~block_signals();

    private:
        sigset_t oldset;
    };
}

#endif

