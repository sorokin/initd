#ifndef FUNCTION_QUEUE_H
#define FUNCTION_QUEUE_H

#include <deque>
#include <functional>

#include "epoll.h"
#include "file_descriptor.h"

namespace sysapi
{
    struct function_queue
    {
        typedef std::function<void ()> function_t;

        function_queue(epoll& ep);

        void push(function_t f);

    private:
        epoll* ep;
        std::pair<file_descriptor, file_descriptor> pipe;
        std::deque<function_t> queue;
        epoll_registration epr;
    };
}
#endif
