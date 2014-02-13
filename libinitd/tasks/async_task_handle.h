#ifndef ASYNC_TASK_HANDLE_H
#define ASYNC_TASK_HANDLE_H

#include <string>
#include "async_task_handle_fwd.h"

struct async_task_handle
{
    enum state
    {
        RUNNING,
        STOPPED,
    };

    virtual ~async_task_handle();

    virtual void set_should_work(bool) = 0;
    virtual bool get_should_work() = 0;

    virtual bool is_running() = 0;
    virtual bool is_in_transition() = 0;

    virtual std::string status_message() const = 0;
};

#endif
