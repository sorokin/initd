#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>

#include <functional>
#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace sysapi
{
    struct child_status {
        const pid_t pid;
        const int status;

        child_status(pid_t pid, int status);
    };

    pid_t fork();
    pid_t fork(std::function<void ()> const& child);

    void waitpid(pid_t);
    boost::optional<child_status> reap_child();

    void execv(std::string const& executable, std::vector<std::string> const& arguments) __attribute__((noreturn));
}

#endif

