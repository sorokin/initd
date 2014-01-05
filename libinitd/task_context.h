#ifndef TASK_CONTEXT_H
#define TASK_CONTEXT_H

namespace sysapi
{
    struct epoll;
}

struct state_context;

struct task_context
{
    virtual sysapi::epoll& get_epoll() = 0;
    virtual state_context& get_state_context() = 0;
};

#endif
