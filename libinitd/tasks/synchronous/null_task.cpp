#include "null_task.h"

#include "task_handle.h"
#include "make_unique.h"

#include <string>

namespace
{
    struct null_task_handle : task_handle
    {
        std::string status_message() const
        {
            return "null task";
        }
    };
}

task_handle_ptr create_task(null_task_data const&)
{
    return make_unique<null_task_handle>();
}

