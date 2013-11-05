#ifndef ASYNC_START_STOP_TASK_H
#define ASYNC_START_STOP_TASK_H

#include <boost/optional.hpp>

#include "async_task_handle.h"
#include "start_stop_task_data.h"

#include "epoll.h"
#include "sigchild_handler.h"

struct async_start_stop_task : async_task_handle
{
    typedef std::function<void ()> current_state_changed_t;

    async_start_stop_task(sysapi::epoll& ep,
                          current_state_changed_t current_state_changed,
                          start_stop_task_data task_data);
    ~async_start_stop_task();

    void set_should_work(bool);
    bool get_should_work();

    bool is_running();
    bool is_in_transition();

    std::string status_message() const;

private:
    void        enqueue_job();

private:
    sysapi::epoll&                      ep;
    current_state_changed_t             current_state_changed;
    start_stop_task_data                task_data;
    bool                                running;
    bool                                should_work;
    boost::optional<sysapi::wait_child> current_operation;
};

#endif
