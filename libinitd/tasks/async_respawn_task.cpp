#include "async_respawn_task.h"

#include "process.h"
#include "current_directory.h"
#include "task_context.h"
#include "make_unique.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <utility>
#include <thread>

#include <boost/ref.hpp>

async_respawn_task::async_respawn_task(task_context& ctx,
                                       current_state_changed_t current_state_changed,
                                       respawn_task_data task_data)
    : ep(ctx.get_epoll())
    , current_state_changed(current_state_changed)
    , task_data(std::move(task_data))
    , running(false)
    , should_work(false)
    , respawn_count(0)
{}

async_respawn_task::~async_respawn_task()
{
    assert(!running);
}

void async_respawn_task::set_should_work(bool s)
{
    should_work = s;
    enqueue_job();
}

bool async_respawn_task::get_should_work()
{
    return should_work;
}

bool async_respawn_task::is_running()
{
    return running;
}

bool async_respawn_task::is_in_transition()
{
    return killer;
}

std::string async_respawn_task::status_message() const
{
    if (running)
    {
        if (should_work)
            return "running";
        else
            return "stopping";
    }
    else
    {
        if (should_work)
            return "starting";
        else
            return "stopped";
    }
}

void async_respawn_task::enqueue_job()
{
    if (running == should_work)
        return;

    if (!should_work)
        std::cerr << "task " << task_data.start.cmd.executable << " is due to be shut down" << std::endl;

    if (should_work && respawn_count < 3)
    {
        pid_t pid = sysapi::fork([this]() {
            ::ptrace(PTRACE_TRACEME, 0, 0, 0);
            ::raise(SIGSTOP);
            sysapi::set_current_directory(task_data.start.working_directory);
            sysapi::execv(task_data.start.cmd.executable, task_data.start.cmd.arguments);
        });

        ::waitpid(pid, 0, 0);
        ::ptrace(PTRACE_SETOPTIONS, pid, 0,
                 PTRACE_O_TRACEFORK |
                 PTRACE_O_TRACEVFORK |
                 PTRACE_O_TRACECLONE);

        add_child_listener(pid);
        ::ptrace(PTRACE_CONT, pid, 0, 0);
        respawn_count++;
        running = true;

        this->current_state_changed();

    } else if (!should_work && !killer) {
        std::cerr << "task " << task_data.start.cmd.executable << " is being shut down" << std::endl;
        if (task_data.stop) {
            std::cerr << "calling the task " << task_data.stop->cmd.executable << " command" << std::endl;
            pid_t halt_pid = sysapi::fork([this]() {
                sysapi::set_current_directory(task_data.stop->working_directory);
                sysapi::execv(task_data.stop->cmd.executable, task_data.stop->cmd.arguments);
            });

            if (task_data.timeout) {
                std::cerr << "task " << task_data.start.cmd.executable << " has death timeout of " << *(task_data.timeout) << std::endl;
                killer = boost::in_place(boost::ref(ep), timespec{*(task_data.timeout), 0}, [this, halt_pid](uint64_t skipped) {
                    ::kill(halt_pid, SIGKILL);
                    for (auto it = children.begin(); it != children.end(); it++) {
                        ::kill(it->first, SIGKILL);
                    }

                    this->shutdown_checklist();
                });
            }

        } else {
            std::cerr << "using SIGTERM method" << std::endl;
            for (auto it = children.begin(); it != children.end(); it++) {
                ::kill(it->first, SIGTERM);
            }

            if (task_data.timeout) {
                    killer = boost::in_place(boost::ref(ep), timespec{*(task_data.timeout), 0}, [this](uint64_t skipped){
                    for (auto it = children.begin(); it != children.end(); it++) {
                        ::kill(it->first, SIGKILL);
                    }

                    this->shutdown_checklist();
                });
            }
        }
    }
}

void async_respawn_task::add_child_listener(pid_t pid) {
    children.insert(std::make_pair(pid, make_unique<sysapi::wait_child>(pid, [this, pid](int status){
                        process_sigtrap(pid, status);
                     })));
}

void async_respawn_task::shutdown_checklist() {
    assert(!this->should_work);
    this->children.clear();
    this->killer = boost::none;
    if (this->running) {
        this->running = false;
        this->current_state_changed();
    }
}

void async_respawn_task::remove_child_listener(pid_t pid) {
    children.erase(pid);
    if (should_work && children.size() == 0) {
        this->running = false;
        enqueue_job();
    } else if (!should_work && running && children.size() == 0) {
        this->shutdown_checklist();
    }
}

void async_respawn_task::process_sigtrap(pid_t pid, int status) {
    if (WIFSTOPPED(status)) {
        if (WSTOPSIG(status) == SIGTRAP) {
            if ((status >> 8) & (PTRACE_EVENT_FORK << 8)) {
                unsigned long grandchild_pid;
                ::ptrace(PTRACE_GETEVENTMSG, pid, 0, &grandchild_pid);
                ::ptrace(PTRACE_SETOPTIONS, grandchild_pid, 0,
                         PTRACE_O_TRACEFORK |
                         PTRACE_O_TRACEVFORK |
                         PTRACE_O_TRACECLONE);

                if (grandchild_pid != 0) add_child_listener(grandchild_pid);

                ptrace(PTRACE_CONT, grandchild_pid, 0, 0);
            }
        } else {
        }
        ptrace(PTRACE_CONT, pid, 0, 0);
    } else if (WIFEXITED(status)) {
        remove_child_listener(pid);
    }
}
