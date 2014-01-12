#ifndef TASK_FWD_H
#define TASK_FWD_H

#include <memory>

struct task;
typedef std::unique_ptr<task> task_sp;

#endif
