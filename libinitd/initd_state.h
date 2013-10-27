#ifndef INITD_STATE_H
#define INITD_STATE_H

#include "task_descriptions.h"
#include "task.h"
#include "run_level.h"
#include "task_queue.h"

struct initd_state
{
    initd_state(task_descriptions&& descriptions);

    void set_run_level(run_level const& rlevel);

    bool has_pending_operations();
    void run_once();

    task_descriptions const& get_descriptions() const;
    tasks             const& get_running() const;

private:
    task_descriptions descriptions;
    tasks             running;
    run_level         current_init_level;
    task_queue_t      pending_operations;
};

#endif // INITD_STATE_H
