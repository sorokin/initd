#ifndef EVENTFD_H
#define EVENTFD_H

#include "file_descriptor.h"
#include "epoll.h"

#include <sys/eventfd.h>
#include <functional>

namespace sysapi
{
    file_descriptor open_eventfd();

    struct eventfd
    {
        eventfd(epoll& ep, std::function<void ()> const& callback);

        void notify();

        eventfd(eventfd const&) = delete;
        eventfd& operator=(eventfd const&) = delete;

    private:
        file_descriptor efd;
        epoll_registration epr;
    };
}

#endif
