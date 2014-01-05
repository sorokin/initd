#ifndef CONTROL_TASK_H
#define CONTROL_TASK_H

#include "task_handle_fwd.h"
#include "control_task_data.h"
#include "epoll.h"

struct task_context;

task_handle_ptr create_task(task_context& ctx, control_task_data const&);

#endif
