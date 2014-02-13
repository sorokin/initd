#ifndef TASK_HANDLE_H
#define TASK_HANDLE_H

#include <memory>
#include <string>
#include "task_handle_fwd.h"

struct task_handle
{
    virtual ~task_handle();

    virtual std::string status_message() const = 0;
};

#endif // TASK_HANDLE_H
