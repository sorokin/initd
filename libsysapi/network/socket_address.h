#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include <sys/socket.h>
#include <string>
#include <ostream>

namespace sysapi
{
    struct socket_address
    {
        socket_address();

        int get_address_family() const;

        void* get_data();
        void const* get_data() const;
        size_t get_size() const;

        static socket_address unix_domain(std::string const& path);

    private:
        sockaddr_storage storage;
    };

    std::ostream& operator<<(std::ostream& s, socket_address const& addr);
}

#endif
