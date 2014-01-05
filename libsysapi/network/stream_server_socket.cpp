#include "stream_server_socket.h"

#include <sys/epoll.h>

#include <cassert>

#include "socket_common.h"

using namespace sysapi;

namespace
{
    file_descriptor prepare_socket(socket_address const& local_address)
    {
        file_descriptor fd = create_socket(local_address.get_address_family(), SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        bind_socket(fd, local_address);
        listen_socket(fd);
        return std::move(fd);
    }
}

stream_server_socket::stream_server_socket(epoll& ep,
                                           socket_address const& local_address,
                                           on_accept_t on_accept)
    : ep(ep)
    , on_accept(std::move(on_accept))
    , fd(prepare_socket(local_address))
    , epr(ep, fd.getfd(), EPOLLIN, [this](uint32_t event) {
        assert(event == EPOLLIN);

        file_descriptor newfd = accept_socket(fd);
        if (newfd.getfd() == -1)
            return;

        this->on_accept(std::move(newfd));
    })
{

}
