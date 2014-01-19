#ifndef INITD_STATE2_H
#define INITD_STATE2_H

#include <vector>
#include <map>

#include "epoll.h"

#include "task_descriptions.h"
#include "run_level.h"

#include "tasks/async_task_handle.h"
#include "task_context.h"
#include "task_fwd.h"

struct state_context;

struct initd_state : private task_context
{
    initd_state(state_context& ctx, sysapi::epoll& ep, task_descriptions descriptions);
    ~initd_state();

    void set_run_level(std::string const& run_level_name);
    void set_empty_run_level();

    bool has_pending_operations() const;

private:
    void clear_should_work_flag();
    void enqueue_all();
    void enqueue_all(std::vector<task*> const&);

private:
    // task_context
    sysapi::epoll& get_epoll();
    state_context& get_state_context();

private:
    state_context&                              ctx;
    sysapi::epoll&                              ep;
    std::vector<task_sp>                        tasks;
    std::map<std::string, std::vector<task*> >  run_levels;
    size_t                                      pending_tasks;

    friend struct task;
};

#endif
