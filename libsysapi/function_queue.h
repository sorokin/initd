#ifndef FUNCTION_QUEUE_H
#define FUNCTION_QUEUE_H

#include <deque>
#include <functional>

#include "epoll.h"
#include "eventfd.h"

namespace sysapi
{
    struct function_queue
    {
        typedef std::function<void ()> function_t;

        function_queue(epoll& ep);

        void push(function_t f);

        function_queue(function_queue const&) = delete;
        function_queue& operator=(function_queue const&) = delete;

    private:
        eventfd efd;
        std::deque<function_t> queue;
    };
}
#endif
