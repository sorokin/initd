#ifndef ASYNC_TASK_HANDLE_FWD_H
#define ASYNC_TASK_HANDLE_FWD_H

#include <memory>

struct async_task_handle;

typedef std::unique_ptr<async_task_handle> async_task_handle_sp;

#endif

