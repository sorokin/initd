#ifndef SIGNALFD_H
#define SIGNALFD_H

#include "file_descriptor.h"
#include "epoll.h"

#include <sys/signalfd.h>
#include <functional>

namespace sysapi
{
    file_descriptor open_signalfd(const sigset_t& mask, int flags);
    sigset_t make_sigset(std::initializer_list<int> const&);

    struct signalfd
    {
        signalfd(epoll& ep, const sigset_t& mask, std::function<void (signalfd_siginfo const&)> const& callback);

    private:
        file_descriptor sfd;
        epoll_registration epr;
    };

    struct block_signals
    {
        block_signals(const sigset_t&);
        ~block_signals();

    private:
        sigset_t oldset;
    };
}

#endif
