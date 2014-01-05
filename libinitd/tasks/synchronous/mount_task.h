#ifndef MOUNT_TASK_H
#define MOUNT_TASK_H

#include "task_handle_fwd.h"
#include "mount_task_data.h"
#include "epoll.h"

struct task_context;

task_handle_ptr create_task(task_context& ctx, mount_task_data const&);
task_handle_ptr create_loop_mount_task(task_context& ctx, mount_task_data const&);

#endif // MOUNT_TASK_H
