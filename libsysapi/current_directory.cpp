#include "current_directory.h"

#include "unistd.h"
#include <cassert>
#include <sstream>
#include <stdexcept>

#include "errors.h"

std::string sysapi::get_current_directory()
{
    size_t N = 4 * 1024;

    char buf[N];
    char* r = ::getcwd(buf, N);
    if (r == nullptr)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to get current directory, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    assert(r == buf);

    return buf;
}

void sysapi::set_current_directory(std::string const& dir)
{
    int r = ::chdir(dir.c_str());

    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to set current directory to \"" << dir << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}
