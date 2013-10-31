#include "epoll.h"

#include <sys/epoll.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "errors.h"

using namespace sysapi;

epoll::epoll()
    : fd_(::epoll_create1(EPOLL_CLOEXEC))
{
    if (fd_.getfd() == -1)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to create epoll object, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

epoll::epoll(epoll&& rhs)
    : fd_(std::move(rhs.fd_))
{}

epoll& epoll::operator=(epoll rhs)
{
    swap(rhs);
    return *this;
}

void epoll::swap(epoll& other)
{
    fd_.swap(other.fd_);
}

void epoll::wait()
{
    epoll_event ev;

    for (;;)
    {
        int r = ::epoll_wait(fd_.getfd(), &ev, 1, -1);

        if (r < 0)
        {
            int err = errno;

            if (err == EINTR)
                continue;

            std::stringstream ss;
            ss << "unable to wait for epoll, error: " << sysapi::errno_to_text(err);

            throw std::runtime_error(ss.str());
        }

        assert(r == 1);
        break;
    }

    try
    {
        static_cast<epoll_registration*>(ev.data.ptr)->callback(ev.events);
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
}

void epoll::add(int fd, uint32_t events, epoll_registration* reg)
{
    epoll_event ev = {0};
    ev.data.ptr = reg;
    ev.events   = events;

    int r = ::epoll_ctl(fd_.getfd(), EPOLL_CTL_ADD, fd, &ev);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to add file descriptor to epoll, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

void epoll::remove(int fd)
{
    int r = ::epoll_ctl(fd_.getfd(), EPOLL_CTL_DEL, fd, nullptr);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to remove file descriptor from epoll, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

epoll_registration::epoll_registration()
    : ep()
    , fd(-1)
{}

epoll_registration::epoll_registration(epoll& ep, int fd, uint32_t events, callback_t callback)
    : ep(&ep)
    , fd(fd)
    , callback(callback)
{
    ep.add(fd, events, this);
}

epoll_registration::epoll_registration(epoll_registration&& rhs)
    : ep(rhs.ep)
    , fd(rhs.fd)
    , callback(std::move(rhs.callback))
{
    rhs.ep = nullptr;
    rhs.fd = -1;
}

epoll_registration::~epoll_registration()
{
    if (ep)
        ep->remove(fd);
}

epoll_registration& epoll_registration::operator=(epoll_registration rhs)
{
    swap(rhs);
    return *this;
}

void epoll_registration::swap(epoll_registration& other)
{
    std::swap(ep, other.ep);
    std::swap(fd, other.fd);
}
