#include "socket_common.h"

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <sys/ioctl.h>

#include "errors.h"

using namespace sysapi;

file_descriptor sysapi::create_socket(int domain, int type, int protocol)
{
    int fd = ::socket(domain, type, protocol);
    if (fd < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to create socket, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(fd);
}

void sysapi::bind_socket(file_descriptor& fd, socket_address const& address)
{
    int r = ::bind(fd.getfd(), static_cast<sockaddr const*>(address.get_data()), address.get_size());
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to bind socket, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

void sysapi::listen_socket(file_descriptor& fd)
{
    int r = ::listen(fd.getfd(), SOMAXCONN);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to listen socket, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

file_descriptor sysapi::accept_socket(file_descriptor const& server_fd)
{
    int fd = ::accept4(server_fd.getfd(), nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (fd < 0)
    {
        int err = errno;

        if (err == ECONNABORTED)
            return file_descriptor();

        std::stringstream ss;
        ss << "unable to create socket, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(fd);
}

void sysapi::connect_socket(file_descriptor& fd, socket_address const& address)
{
    int r = ::connect(fd.getfd(), static_cast<sockaddr const*>(address.get_data()), address.get_size());
    if (r < 0)
    {
        int err = errno;

        if (err == EINPROGRESS)
            return;

        std::stringstream ss;
        ss << "unable to connect to \"" << address << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

size_t sysapi::socket_avail_for_read(file_descriptor const& fd)
{
    int avail;
    int r = ::ioctl(fd.getfd(), FIONREAD, &avail);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to get available bytes, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    assert(avail >= 0);
    return (size_t)avail;
}
