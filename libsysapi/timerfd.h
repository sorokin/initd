#ifndef TIMERFD_H
#define TIMERFD_H

#include "file_descriptor.h"
#include "epoll.h"

#include <functional>

namespace sysapi
{
    file_descriptor open_timerfd(timespec internal);

    struct timerfd
    {
        timerfd(epoll& ep, timespec interval, std::function<void (uint64_t)> const& on_tick);

        timerfd(timerfd const&) = delete;
        timerfd& operator=(timerfd const&) = delete;

    private:
        file_descriptor sfd;
        epoll_registration epr;
    };
}

#endif
