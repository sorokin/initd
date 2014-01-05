#include "stream_socket.h"
#include "socket_common.h"

#include <sys/epoll.h>
#include <cassert>
#include <iostream>

using namespace sysapi;

namespace
{
    file_descriptor prepare_socket(socket_address const& remote_address)
    {
        file_descriptor fd = create_socket(remote_address.get_address_family(), SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        connect_socket(fd, remote_address);

        return std::move(fd);
    }
}

stream_socket::stream_socket()
    : ep(nullptr)
{}

stream_socket::~stream_socket()
{
    if (destroyed_tag)
        *destroyed_tag = true;
}

stream_socket::stream_socket(epoll& ep, file_descriptor fd)
    : ep(&ep)
    , fd(std::move(fd))
{}

stream_socket::stream_socket(stream_socket&& rhs)
    : ep(rhs.ep)
    , fd(std::move(rhs.fd))
    , epr(std::move(rhs.epr))
{
    rhs.ep = nullptr;
}

size_t stream_socket::avail() const
{
    return socket_avail_for_read(fd);
}

void stream_socket::read(void* data, size_t size)
{
    fd.read(data, size);
}

void stream_socket::set_on_read(callback_t on_read)
{
    this->on_read = on_read;
    update();
}

void stream_socket::set_on_disconnect(callback_t on_disconnect)
{
    this->on_disconnect = on_disconnect;
    update();
}

void stream_socket::update()
{
    uint32_t events = uint32_t();
    if (on_read)
        events |= EPOLLIN;
    if (on_disconnect)
        events |= EPOLLHUP;

    epr.clear();

    if (events)
    {
        epr = epoll_registration(*ep, fd.getfd(), events, [this](uint32_t events) {
            bool destroyed = false;
            destroyed_tag = &destroyed;

            try
            {
                if (events & EPOLLIN)
                {
                    if (on_read)
                    {
                        on_read();
                        if (destroyed)
                            return;
                    }
                    if (destroyed)
                        return;

                    events &= ~EPOLLIN;
                }

                if (events & EPOLLHUP)
                {
                    if (on_disconnect) // EPOLLHUP can be send even if we are not subscribed for it
                    {
                        on_disconnect();
                        if (destroyed)
                            return;
                    }

                    events &= ~EPOLLHUP;
                }

                if (events)
                {
                    std::cerr << "unknown events type: " << events << std::endl;
                }
            }
            catch (std::exception const& e)
            {
                if (!destroyed)
                    destroyed_tag = nullptr;

                throw e;
            }
        });
    }
}

read_process::read_process(stream_socket& ss, on_ready_t on_ready)
    : socket(&ss)
    , on_ready(std::move(on_ready))
{
    socket->set_on_read([this]() {
        size_t avail = socket->avail();
        size_t offset = buffer.size();
        buffer.resize(buffer.size() + avail);
        socket->read(static_cast<char*>(buffer.data()) + offset, avail);
        this->on_ready();
    });
}

read_process::~read_process()
{
    socket->set_on_read(stream_socket::callback_t());
}

void read_process::clear_buffer()
{
    buffer.clear();
}

void const* read_process::data() const
{
    return buffer.data();
}

size_t read_process::size() const
{
    return buffer.size();
}

connection_process::connection_process(epoll& ep,
                                       socket_address const& remote_address,
                                       on_connect_t on_connect)
    : ep(ep)
    , on_connect(std::move(on_connect))
    , fd(prepare_socket(remote_address))
    , epr(ep, fd.getfd(), EPOLLOUT, [this](uint32_t events) {
        assert(events == EPOLLOUT);

        epr.clear();
        this->on_connect(stream_socket(this->ep, std::move(fd)));
    })
{

}
