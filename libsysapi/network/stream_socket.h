#ifndef STREAM_SOCKET_H
#define STREAM_SOCKET_H

#include "epoll.h"
#include "socket_address.h"

namespace sysapi
{
    struct stream_socket
    {
        typedef std::function<void ()> callback_t;

        stream_socket();
        ~stream_socket();
        stream_socket(epoll& ep, file_descriptor fd);
        stream_socket(stream_socket&& rhs);
        stream_socket(stream_socket const& rhs) = delete;

        stream_socket& operator=(stream_socket const&) = delete;

        void read(void* data, size_t size);
        size_t avail() const;
        void set_on_read(callback_t on_read);
        void set_on_disconnect(callback_t on_disconnect);

    private:
        void update();

    private:
        epoll* ep;
        file_descriptor fd;
        epoll_registration epr;
        callback_t on_read;
        callback_t on_disconnect;
        bool* destroyed_tag;
    };

    struct read_process
    {
        typedef std::function<void ()> on_ready_t;

        read_process(stream_socket&, on_ready_t);
        ~read_process();
        read_process(read_process const&) = delete;

        read_process& operator=(read_process const&) = delete;

        void clear_buffer();
        void const* data() const;
        size_t size() const;

    private:
        stream_socket* socket;
        on_ready_t on_ready;
        std::vector<char> buffer;

        friend struct stream_socket;
    };

    struct connection_process
    {
        typedef std::function<void (stream_socket)> on_connect_t;

        connection_process(epoll& ep,
                           socket_address const& remote_address,
                           on_connect_t on_connect);

        connection_process(connection_process const&) = delete;
        connection_process& operator=(connection_process const&) = delete;

    private:
        epoll& ep;
        on_connect_t on_connect;
        file_descriptor fd;
        epoll_registration epr;
    };
}

#endif
