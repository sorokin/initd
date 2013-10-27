#include "failed_task.h"

#include "task_handle.h"
#include "make_unique.h"

namespace
{
    struct failed_task_handle : task_handle
    {
        failed_task_handle(failed_task_data const& data)
            : data(data)
        {}

        std::string status_message() const
        {
            return data.status_message;
        }

    private:
        failed_task_data data;
    };
}

task_handle_ptr create_failed_task(failed_task_data const& data)
{
    return make_unique<failed_task_handle>(data);
}
