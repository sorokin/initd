#ifndef HOSTNAME_TASK_H
#define HOSTNAME_TASK_H

#include "task_handle_fwd.h"
#include "hostname_task_data.h"
#include "epoll.h"

struct task_context;

task_handle_ptr create_task(task_context& ctx, hostname_task_data const&);

#endif // HOSTNAME_TASK_H
