#include "loop_device.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/loop.h>

#include <sstream>
#include <stdexcept>

#include "errors.h"

using namespace sysapi;

loop_device::loop_device(std::string const& filename)
    : fd_(file_descriptor::open(filename, O_RDWR | O_CLOEXEC))
{}

void loop_device::setfd(int fd)
{
    int r = ioctl(fd_.getfd(), LOOP_SET_FD, fd);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to ioctl LOOP_SET_FD, error: " << sysapi::errno_to_text(err);

        switch (err)
        {
        case EBUSY:
            ss << ", loop device is already in use";
            break;
        }

        throw std::runtime_error(ss.str());
    }
}

void loop_device::setfd(file_descriptor const& fd)
{
    setfd(fd.getfd());
}

bool loop_device::try_setfd(int fd)
{
    int r = ioctl(fd_.getfd(), LOOP_SET_FD, fd);
    if (r < 0)
    {
        int err = errno;
        if (err == EBUSY)
            return false;

        std::stringstream ss;
        ss << "unable to ioctl LOOP_SET_FD, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return true;
}

bool loop_device::try_setfd(file_descriptor const& fd)
{
    return try_setfd(fd.getfd());
}

void loop_device::clearfd()
{
    int r = ioctl(fd_.getfd(), LOOP_CLR_FD, 0);
    if (r < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to ioctl LOOP_CLR_FD, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }
}
