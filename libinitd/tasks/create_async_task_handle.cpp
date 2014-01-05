#include "create_async_task_handle.h"

#include "sync_task_adaptor.h"
#include "async_start_stop_task.h"

#include "make_unique.h"

async_task_handle_sp create_async_task_handle(task_context& ctx, std::function<void ()> state_changed, task_data const& td)
{
    if (mount_task_data const* d = boost::get<mount_task_data>(&td))
        return make_unique<sync_task_adaptor<mount_task_data> >(ctx, state_changed, *d);
    else if (hostname_task_data const* d = boost::get<hostname_task_data>(&td))
        return make_unique<sync_task_adaptor<hostname_task_data> >(ctx, state_changed, *d);
    else if (start_stop_task_data const* d = boost::get<start_stop_task_data>(&td))
        return make_unique<async_start_stop_task>(ctx, state_changed, *d);
    else if (control_task_data const* d = boost::get<control_task_data>(&td))
        return make_unique<sync_task_adaptor<control_task_data> >(ctx, state_changed, *d);
    else if (null_task_data const* d = boost::get<null_task_data>(&td))
        return make_unique<sync_task_adaptor<null_task_data> >(ctx, state_changed, *d);
    else
    {
        assert(false);
        return async_task_handle_sp();
    }
}
