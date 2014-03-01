#ifndef ASYNC_RESPAWN_TASK_H
#define ASYNC_RESPAWN_TASK_H

#include <boost/optional.hpp>

#include "async_task_handle.h"
#include "respawn_task_data.h"

#include "epoll.h"
#include "sigchild_handler.h"
#include "timerfd.h"
#include "process.h"

#include <list>
#include <map>
#include <thread>

struct task_context;

struct async_respawn_task : async_task_handle
{
    typedef std::function<void ()> current_state_changed_t;

    async_respawn_task(task_context& ctx,
                          current_state_changed_t current_state_changed,
                          respawn_task_data task_data);
    ~async_respawn_task();

    void set_should_work(bool);
    bool get_should_work();

    bool is_running();
    bool is_in_transition();

    std::string status_message() const;

private:
    void        enqueue_job();

private:
    void process_sigtrap(pid_t pid, int status);
    void add_child_listener(pid_t pid);
    void remove_child_listener(pid_t pid);
    void shutdown_checklist();

    sysapi::epoll&                      ep;
    current_state_changed_t             current_state_changed;
    respawn_task_data                   task_data;
    bool                                running;
    bool                                should_work;
    int                                 respawn_count;
    boost::optional<sysapi::timerfd>    killer;
    std::map<pid_t, std::unique_ptr<sysapi::wait_child>> children;

};

#endif // ASYNC_RESPAWN_TASK_H
