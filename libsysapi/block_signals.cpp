#include "block_signals.h"

#include <errno.h>

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "errors.h"

using namespace sysapi;

sigset_t sysapi::make_sigset(std::initializer_list<int> const& ss)
{
    sigset_t set;
    sigemptyset(&set);

    for (int signo : ss)
        sigaddset(&set, signo);

    return set;
}

sigset_t sysapi::make_sigset(int signo)
{
    sigset_t set;
    sigemptyset(&set);

    sigaddset(&set, signo);

    return set;
}

block_signals::block_signals(const sigset_t& mask)
{
    int r = ::sigprocmask(SIG_BLOCK, &mask, &oldset);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to block signals, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

block_signals::~block_signals()
{
    int r = ::sigprocmask(SIG_SETMASK, &oldset, nullptr);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to unblock signals, error: " << sysapi::errno_to_text(err);

        std::cerr << ss.str() << std::endl;
    }
}
