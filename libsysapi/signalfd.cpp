#include "signalfd.h"

#include <sys/epoll.h>
#include <signal.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "errors.h"

using namespace sysapi;

file_descriptor sysapi::open_signalfd(const sigset_t& mask, int flags)
{
    int r = ::signalfd(-1, &mask, flags);

    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to create signalfd, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(r);
}

sigset_t sysapi::make_sigset(std::initializer_list<int> const& ss)
{
    sigset_t set;
    sigemptyset(&set);

    for (int signo : ss)
        sigaddset(&set, signo);

    return set;
}

signalfd::signalfd(epoll& ep, const sigset_t& mask, std::function<void (signalfd_siginfo const&)> const& callback)
    : sfd(open_signalfd(mask, SFD_CLOEXEC | SFD_NONBLOCK))
    , epr(ep, sfd.getfd(), EPOLLIN, [this, callback](uint32_t events) {
        assert(events == EPOLLIN);

        signalfd_siginfo info;
        sfd.read(&info, sizeof info);
        callback(info);
    })
{}

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
