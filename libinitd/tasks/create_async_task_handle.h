#ifndef CREATE_ASYNC_TASK_HANDLE_H
#define CREATE_ASYNC_TASK_HANDLE_H

#include <functional>

#include "async_task_handle_fwd.h"
#include "task_data.h"

#include "epoll.h"

struct task_context;

async_task_handle_sp create_async_task_handle(task_context&, std::function<void ()> state_changed, task_data const&);

#endif

