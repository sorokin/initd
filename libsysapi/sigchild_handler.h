#ifndef SIGCHILD_HANDLER_H
#define SIGCHILD_HANDLER_H

#include <memory>
#include <functional>

namespace sysapi
{
    struct epoll;

    struct sigchild_handler;

    struct wait_child
    {
        typedef std::function<void ()> callback_t;

        wait_child(pid_t pid, callback_t callback);
        wait_child(wait_child const&) = delete;
        ~wait_child();

        wait_child& operator=(wait_child const&) = delete;

    private:
        pid_t pid;
        callback_t callback;

        friend struct sigchild_handler;
    };

    struct install_sigchild_handler
    {
        install_sigchild_handler(epoll&);
        install_sigchild_handler(install_sigchild_handler const&) = delete;
        ~install_sigchild_handler();

        install_sigchild_handler& operator=(install_sigchild_handler) = delete;

    private:
        std::unique_ptr<sigchild_handler> handler;
    };
}

#endif
