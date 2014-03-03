#include "sigchild_handler.h"

#include <signal.h>

#include <cassert>
#include <map>
#include <thread>

#include "signalfd.h"
#include "epoll.h"
#include "block_signals.h"
#include "process.h"
#include "function_queue.h"

using namespace sysapi;

struct sysapi::sigchild_handler
{
    sigchild_handler(epoll& ep)
        : fq(function_queue(ep))
    {
        struct sigaction act;
        act.sa_sigaction = [](int signo, siginfo_t* sg, void* ctx) {
            sigchild_handler::instance->handle(signo, sg);
        };
        act.sa_mask = make_sigset(SIGCHLD);
        act.sa_flags |= SA_SIGINFO;

        //act.sa_flags = SA_NOCLDWAIT;
        ::sigaction (SIGCHLD, &act, NULL);
    }

    ~sigchild_handler() {
        struct sigaction act;
        act.sa_handler = SIG_DFL;
        act.sa_mask = make_sigset(SIGCHLD);

        ::sigaction (SIGCHLD, &act, NULL);
    }

    void add(pid_t pid, wait_child* wc)
    {
        assert(handlers.find(pid) == handlers.end());

        handlers.insert(std::make_pair(pid, wc));
    }

    void remove(pid_t pid)
    {
        auto i = handlers.find(pid);
        assert(i != handlers.end());
        handlers.erase(i);
    }

    void handle(int signal, siginfo_t* sg) {
        block_signals siglock(make_sigset(SIGCHLD));
        fq.push([this, sg](){
            for (;;)
            {
                boost::optional<sysapi::child_status> child = reap_child();
                if (!child)
                {
                    break;
                }

                auto i = handlers.find(child->pid);
                if (i != handlers.end())
                    i->second->callback(*child);
                return;
            }
        });
    }

    static sigchild_handler* instance;
    function_queue fq;

private:
    std::map<pid_t, wait_child*> handlers;
    // signalfd sfd;
};

sigchild_handler* sigchild_handler::instance = nullptr;

wait_child::wait_child(pid_t pid, detailed_callback_t callback)
    : pid(pid)
    , callback(callback)
{
    assert(sigchild_handler::instance);

    sigchild_handler::instance->add(pid, this);
}

wait_child::~wait_child()
{
    assert(sigchild_handler::instance);

    sigchild_handler::instance->remove(pid);
}

install_sigchild_handler::install_sigchild_handler(epoll& ep)
{
    assert(!sigchild_handler::instance);

    handler.reset(new sigchild_handler(ep));
    sigchild_handler::instance = handler.get();
}

install_sigchild_handler::~install_sigchild_handler()
{
    assert(sigchild_handler::instance);
    sigchild_handler::instance = nullptr;
}

