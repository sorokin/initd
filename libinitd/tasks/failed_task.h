#ifndef FAILED_TASK_H
#define FAILED_TASK_H

#include <string>
#include "task_handle_fwd.h"

struct failed_task_data
{
    std::string status_message;
};

task_handle_ptr create_failed_task(failed_task_data const&);

#endif // FAILED_TASK_H
