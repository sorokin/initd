#ifndef CURRENT_DIRECTORY_H
#define CURRENT_DIRECTORY_H

#include <string>

namespace sysapi
{
    std::string get_current_directory();
    void set_current_directory(std::string const&);
}

#endif
