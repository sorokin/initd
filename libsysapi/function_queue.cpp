#include "function_queue.h"

#include <sys/epoll.h>
#include <fcntl.h>

#include <cassert>

using namespace sysapi;

function_queue::function_queue(epoll& ep)
    : efd(ep, [this]() {
        function_t f = std::move(queue.front());
        queue.pop_front();

        f();
    })
{}

void function_queue::push(function_t f)
{
    queue.push_back(std::move(f));
    efd.notify();
}
