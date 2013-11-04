#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>

#include <functional>
#include <string>
#include <vector>

namespace sysapi
{
    pid_t fork();
    pid_t fork(std::function<void ()> const& child);

    void waitpid(pid_t);

    void execv(std::string const& executable, std::vector<std::string> const& arguments) __attribute__((noreturn));
}

#endif

