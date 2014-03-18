#include "sigchild_handler.h"

#include <signal.h>

#include <cassert>
#include <map>

#include "signalfd.h"
#include "epoll.h"
#include "block_signals.h"
#include "process.h"

using namespace sysapi;

struct sysapi::sigchild_handler
{
    sigchild_handler(epoll& ep)
        : block(make_sigset(SIGCHLD))
        , sfd(ep, make_sigset(SIGCHLD), [this](signalfd_siginfo const& sinfo) {
            for (;;)
            {
                boost::optional<sysapi::child_status> child = reap_child();
                if (!child)
                    break;

                auto i = handlers.find(child->pid);
                if (i != handlers.end())
                    i->second->callback(child->status);
            }
        })
    {}

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

    static sigchild_handler* instance;

private:
    std::map<pid_t, wait_child*> handlers;
    block_signals block;
    signalfd sfd;
};

sigchild_handler* sigchild_handler::instance = nullptr;

wait_child::wait_child(pid_t pid, callback_t callback)
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
    assert(sigchild_handler::instance == handler.get());
    sigchild_handler::instance = nullptr;
}
