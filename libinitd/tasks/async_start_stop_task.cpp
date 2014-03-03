#include "async_start_stop_task.h"

#include <cassert>

#include "process.h"
#include "current_directory.h"
#include "task_context.h"

async_start_stop_task::async_start_stop_task(task_context& ctx,
                                             current_state_changed_t current_state_changed,
                                             start_stop_task_data task_data)
    : ep(ctx.get_epoll())
    , current_state_changed(current_state_changed)
    , task_data(std::move(task_data))
    , running(false)
    , should_work(false)
{}

async_start_stop_task::~async_start_stop_task()
{
    assert(!running);
    assert(!current_operation);
}

void async_start_stop_task::set_should_work(bool s)
{
    should_work = s;
    enqueue_job();
}

bool async_start_stop_task::get_should_work()
{
    return should_work;
}

bool async_start_stop_task::is_running()
{
    return running;
}

bool async_start_stop_task::is_in_transition()
{
    return current_operation;
}

std::string async_start_stop_task::status_message() const
{
    if (running)
    {
        if (should_work)
            return current_operation ? "restarting" : "running";
        else
            return "stopping";
    }
    else
    {
        if (should_work)
            return "starting";
        else
            return current_operation ? "cancelling" : "stopped";
    }
}

void async_start_stop_task::enqueue_job()
{
    if (current_operation)
        return;

    if (running == should_work)
        return;

    if (should_work)
    {
        pid_t pid = sysapi::fork([this]() {
            sysapi::set_current_directory(task_data.start.working_directory);
            sysapi::execv(task_data.start.cmd.executable, task_data.start.cmd.arguments);
        });

        current_operation = boost::in_place(pid, [this](int /*status*/) {
            async_start_stop_task* that = this;

            assert(!that->running);
            that->running = true;
            that->current_operation = boost::none;
            that->enqueue_job();
            that->current_state_changed();
        });
    }
    else
    {
        pid_t pid = sysapi::fork([this]() {
            sysapi::set_current_directory(task_data.stop.working_directory);
            sysapi::execv(task_data.stop.cmd.executable, task_data.stop.cmd.arguments);
        });

        current_operation = boost::in_place(pid, [this](int /*status*/) {
            async_start_stop_task* that = this;

            assert(that->running);
            that->running = false;
            that->current_operation = boost::none;
            that->enqueue_job();
            that->current_state_changed();
        });
    }
}
