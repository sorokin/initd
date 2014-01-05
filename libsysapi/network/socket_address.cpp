#include "socket_address.h"

#include <sys/un.h>
#include <netinet/in.h>

#include <cassert>
#include <cstring>
#include <iterator>
#include <sstream>
#include <stdexcept>

using namespace sysapi;

socket_address::socket_address()
    : storage()
{}

int socket_address::get_address_family() const
{
    return storage.ss_family;
}

void* socket_address::get_data()
{
    return &storage;
}

void const* socket_address::get_data() const
{
    return &storage;
}

size_t socket_address::get_size() const
{
    switch (get_address_family())
    {
    case AF_UNIX:
        return sizeof(sockaddr_un);
    case AF_INET:
        return sizeof(sockaddr_in);
    case AF_INET6:
        return sizeof(sockaddr_in6);
    default:
        assert(false);
        return 0;
    }
}

socket_address socket_address::unix_domain(std::string const& path)
{
    socket_address r;

    sockaddr_un& saddr = *static_cast<sockaddr_un*>(r.get_data());
    saddr.sun_family = AF_UNIX;

    size_t max_path_size = std::end(saddr.sun_path) - std::begin(saddr.sun_path) - 1; // null-terminated
    if (path.size() > max_path_size)
    {
        std::stringstream ss;
        ss << "too long address \"" << path << "\"";
        throw std::runtime_error(ss.str());
    }

    strcpy(saddr.sun_path, path.c_str());

    return r;
}
