#include "eventfd.h"

#include <sys/epoll.h>
#include <signal.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "errors.h"

using namespace sysapi;

file_descriptor sysapi::open_eventfd()
{
    int r = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to create eventfd, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(r);
}

eventfd::eventfd(epoll& ep, std::function<void ()> const& callback)
    : efd(open_eventfd())
    , epr(ep, efd.getfd(), EPOLLIN, [this, callback](uint32_t events) {
        assert(events == EPOLLIN);

        uint64_t tmp;
        efd.read(&tmp, sizeof tmp);
        callback();
    })
{}

void eventfd::notify()
{
    uint64_t tmp = 1;
    efd.write(&tmp, sizeof tmp);
}
