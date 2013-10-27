#include "initd_state.h"

#include <assert.h>
#include <utility>

initd_state::initd_state(task_descriptions&& d)
    : descriptions(std::move(d))
{
}

void initd_state::set_run_level(run_level const& rlevel)
{
    current_init_level = rlevel;
    pending_operations = calculate_task_queue(running, descriptions, current_init_level);
}

bool initd_state::has_pending_operations()
{
    return !pending_operations.empty();
}

void initd_state::run_once()
{
    if (pending_operations.empty())
    {
        assert(false);
        return;
    }

    task_queue_element const& op = pending_operations.front();

    if (op.start)
    {
        running.insert_task(create_task(op.task));
    }
    else
    {
        running.erase_task(op.task);
    }

    pending_operations.pop_front();
}

task_descriptions const& initd_state::get_descriptions() const
{
    return descriptions;
}

tasks const& initd_state::get_running() const
{
    return running;
}
