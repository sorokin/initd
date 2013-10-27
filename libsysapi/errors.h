#ifndef SYSAPI_H
#define SYSAPI_H

#include <string>

namespace sysapi
{
    std::string errno_to_description(int err);
    std::string errno_to_name(int err);
    std::string errno_to_text(int err);
}

#endif // SYSAPI_H
