#ifndef TASK_HANDLE_FWD_H
#define TASK_HANDLE_FWD_H

#include <memory>

struct task_handle;

typedef std::unique_ptr<task_handle> task_handle_ptr;

#endif // TASK_HANDLE_FWD_H
