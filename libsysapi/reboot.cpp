#include "reboot.h"

#include "errors.h"

#include <sys/reboot.h>
#include <sstream>
#include <stdexcept>

void sysapi::reboot()
{
    int r = ::reboot(RB_AUTOBOOT);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to reboot computer, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}

void sysapi::power_off()
{
    int r = ::reboot(RB_POWER_OFF);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to power off computer, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}
