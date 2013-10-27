#ifndef TASK_DESCRIPTION_FWD_H
#define TASK_DESCRIPTION_FWD_H

#include <memory>

struct task_description;

typedef std::unique_ptr<task_description> task_description_sp;

#endif

