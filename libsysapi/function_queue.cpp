#include "function_queue.h"

#include <sys/epoll.h>
#include <fcntl.h>

#include <cassert>

using namespace sysapi;

function_queue::function_queue(epoll& ep)
    : ep(&ep)
    , pipe(create_pipe(O_NONBLOCK | O_CLOEXEC))
    , epr(ep, pipe.first.getfd(), EPOLLIN, [this](uint32_t events) {
        assert(events == EPOLLIN);

        char dummy;
        pipe.first.read(&dummy, sizeof dummy);

        function_t f = std::move(queue.front());
        queue.pop_front();

        f();
    })
{}

void function_queue::push(function_t f)
{
    queue.push_back(std::move(f));

    char dummy = '\0';
    pipe.second.write(&dummy, sizeof dummy);
}
