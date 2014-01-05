#ifndef SOCKET_COMMON_H
#define SOCKET_COMMON_H

#include "file_descriptor.h"
#include "socket_address.h"

namespace sysapi
{
    file_descriptor create_socket(int domain, int type, int protocol);
    void bind_socket(file_descriptor& fd, socket_address const& address);
    void listen_socket(file_descriptor& fd);
    file_descriptor accept_socket(file_descriptor const& server_fd);
    void connect_socket(file_descriptor& fd, socket_address const& address);
    size_t socket_avail_for_read(file_descriptor const& fd);
}

#endif
