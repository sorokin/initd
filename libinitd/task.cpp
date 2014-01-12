#include "task.h"
#include "tasks/async_task_handle.h"
#include "initd_state.h"

#include <cassert>

void add_task_dependency(task& dependant, task& dependency)
{
    assert(!dependency.handle->is_running());

    dependant.dependencies.push_back(&dependency);
    dependency.dependants.push_back(&dependant);

    ++dependant.stopped_dependencies;
}

task::task(async_task_handle_sp handle)
    : handle(std::move(handle))
    , should_work(false)
    , stopped_dependencies(0)
    , running_dependants(0)
    , counted_in_dependencies(false)
    , counted_in_dependants(false)
    , counted_in_pending_tasks(false)
{}

async_task_handle* task::get_handle()
{
    return handle.get();
}

std::vector<task*> const& task::get_dependencies() const
{
    return dependencies;
}

std::vector<task*> const& task::get_dependants() const
{
    return dependants;
}

bool task::are_dependencies_running() const
{
    return stopped_dependencies == 0;
}

bool task::are_dependants_stopped() const
{
    return running_dependants == 0;
}

void task::clear_should_work()
{
    should_work = false;
}

void task::mark_should_work()
{
    bool old = should_work;

    should_work = true;

    if (!old)
        for (task* dep : dependencies)
            dep->mark_should_work();
}

void task::enqueue_this()
{
    if ((!handle->is_running() || handle->is_in_transition())
        && should_work && are_dependencies_running())
        handle->set_should_work(true);

    if ((handle->is_running() || handle->is_in_transition())
        && !should_work && are_dependants_stopped())
        handle->set_should_work(false);
}

void task::sync(initd_state* istate)
{
    bool affect_dependencies = handle->is_running() || handle->is_in_transition();
    if (affect_dependencies != counted_in_dependencies)
    {
        increment_counter_in_dependencies(affect_dependencies ? 1 : -1);
        counted_in_dependencies = affect_dependencies;
    }

    bool affect_dependants = handle->is_running() && !handle->is_in_transition();
    if (affect_dependants != counted_in_dependants)
    {
        increment_counter_in_dependants(affect_dependants ? -1 : 1);
        counted_in_dependants = affect_dependants;
    }

    bool affect_pending_tasks = handle->is_in_transition() || handle->is_running() != should_work;
    if (affect_pending_tasks != counted_in_pending_tasks)
    {
        istate->pending_tasks += (affect_pending_tasks ? 1 : -1);
        counted_in_pending_tasks = affect_pending_tasks;
    }
}

void task::increment_counter_in_dependencies(std::ptrdiff_t delta)
{
    for (task* dep : dependencies)
        dep->running_dependants += delta;
}

void task::increment_counter_in_dependants(std::ptrdiff_t delta)
{
    for (task* dep : dependants)
        dep->stopped_dependencies += delta;
}
