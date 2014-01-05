#ifndef STREAM_SERVER_SOCKET_H
#define STREAM_SERVER_SOCKET_H

#include "socket_address.h"
#include "epoll.h"
#include "file_descriptor.h"
#include <functional>

namespace sysapi
{
    struct epoll;
    struct stream_socket;

    struct stream_server_socket
    {
        typedef std::function<void (file_descriptor)> on_accept_t;

        stream_server_socket(epoll&,
                             socket_address const& local_address,
                             on_accept_t on_accept);

        stream_server_socket(stream_server_socket const&);
        stream_server_socket& operator=(stream_server_socket const&);

    private:
        epoll&             ep;
        on_accept_t        on_accept;
        file_descriptor    fd;
        epoll_registration epr;
    };
}

#endif
