#include "sync_task_adaptor.h"

#include <cassert>
#include <sstream>
#include <iostream>

#include "task_context.h"
#include "synchronous/task_handle.h"

#include "synchronous/hostname_task.h"
#include "synchronous/mount_task.h"
#include "synchronous/control_task.h"
#include "synchronous/null_task.h"

template <typename TaskData>
sync_task_adaptor<TaskData>::sync_task_adaptor(task_context& ctx,
                                               current_state_changed_t current_state_changed,
                                               TaskData task_data)
    : ctx(ctx)
    , current_state_changed(current_state_changed)
    , task_data(task_data)
    , running(false)
    , fq(ctx.get_epoll())
{}

template <typename TaskData>
sync_task_adaptor<TaskData>::~sync_task_adaptor()
{}

template <typename TaskData>
void sync_task_adaptor<TaskData>::set_should_work(bool s)
{
    running = s;

    if (running)
    {
        if (!handle)
        {
            try
            {
                handle = create_task(ctx, task_data);
            }
            catch (std::exception const& e)
            {
                std::cerr << "can't start task: " << e.what() << std::endl;
            }
        }
        fq.push([this] () {
            current_state_changed();
        });
    }
    else
    {
        handle.reset();
        fq.push([this] () {
            current_state_changed();
        });
    }
}

template <typename TaskData>
bool sync_task_adaptor<TaskData>::get_should_work()
{
    return running;
}

template <typename TaskData>
bool sync_task_adaptor<TaskData>::is_running()
{
    return running;
}

template <typename TaskData>
bool sync_task_adaptor<TaskData>::is_in_transition()
{
    return false;
}

template <typename TaskData>
std::string sync_task_adaptor<TaskData>::status_message() const
{
    if (running)
    {
        std::stringstream ss;
        if (handle)
            ss << "running: " << handle->status_message();
        else
            ss << "error";

        return ss.str();
    }
    else
        return "stopped";
}

template struct sync_task_adaptor<hostname_task_data>;
template struct sync_task_adaptor<mount_task_data>;
template struct sync_task_adaptor<control_task_data>;
template struct sync_task_adaptor<null_task_data>;
