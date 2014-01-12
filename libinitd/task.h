#ifndef TASK_H
#define TASK_H

#include "task_fwd.h"
#include "tasks/async_task_handle_fwd.h"
#include <vector>

struct initd_state;

struct task
{
    task(async_task_handle_sp handle);

    bool are_dependencies_running() const;
    bool are_dependants_stopped() const;

    void mark_should_work();
    void enqueue_this();

    void sync(initd_state* istate);

    friend void add_task_dependency(task& dependant, task& dependency);

private:
    void increment_counter_in_dependencies(std::ptrdiff_t);
    void increment_counter_in_dependants(std::ptrdiff_t);

public:
    async_task_handle_sp handle;
    bool                 should_work;

    std::vector<task*>   dependencies;
    std::vector<task*>   dependants;

    size_t               stopped_dependencies;
    size_t               running_dependants;

    bool                 counted_in_dependencies;
    bool                 counted_in_dependants;
    bool                 counted_in_pending_tasks;
};

#endif
