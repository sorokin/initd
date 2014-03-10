#include "timerfd.h"
#include "errors.h"

#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <cassert>
#include <sstream>
#include <stdexcept>

using namespace sysapi;

file_descriptor sysapi::open_timerfd(timespec interval)
{
    int r = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to create timerfd, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    itimerspec its{interval, interval};
    int rr = ::timerfd_settime(r, 0, &its, nullptr);
    if  (rr < 0)
    {
         int err = errno;

         std::stringstream ss;
         ss << "unable to setup timerfd, error: " << sysapi::errno_to_text(err);

         throw std::runtime_error(ss.str());
    }

    return file_descriptor(r);
}

timerfd::timerfd(epoll& ep, timespec interval, std::function<void (uint64_t)> const& on_tick)
    : sfd(open_timerfd(interval))
    , epr(ep, sfd.getfd(), EPOLLIN, [this, on_tick](uint32_t events) {
        assert(events == EPOLLIN);

        uint64_t number_of_expirations;
        sfd.read(&number_of_expirations, sizeof number_of_expirations);
        on_tick(number_of_expirations);
    })
{}
