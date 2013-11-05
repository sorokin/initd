#ifndef SYNC_TASK_ADAPTOR_H
#define SYNC_TASK_ADAPTOR_H

#include "async_task_handle.h"
#include "synchronous/task_handle_fwd.h"

#include "epoll.h"
#include "function_queue.h"

template <typename TaskData>
struct sync_task_adaptor : async_task_handle
{
    typedef std::function<void ()> current_state_changed_t;

    sync_task_adaptor(sysapi::epoll& ep,
                      current_state_changed_t current_state_changed,
                      TaskData task_data);
    ~sync_task_adaptor();

    void set_should_work(bool);
    bool get_should_work();

    bool is_running();
    bool is_in_transition();

    std::string status_message() const;

private:
    sysapi::epoll& ep;
    current_state_changed_t current_state_changed;
    TaskData task_data;
    bool running;
    task_handle_ptr handle;
    sysapi::function_queue fq;
};

#endif
