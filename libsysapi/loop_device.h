#ifndef LOOP_DEVICE_H
#define LOOP_DEVICE_H

#include <string>

#include "file_descriptor.h"

namespace sysapi
{
    struct loop_device
    {
        loop_device(std::string const& filename);

        void setfd(int);
        void setfd(file_descriptor const&);

        bool try_setfd(int);
        bool try_setfd(file_descriptor const&);

        void clearfd();

    private:
        file_descriptor fd_;
    };
}

#endif // LOOP_DEVICE_H
