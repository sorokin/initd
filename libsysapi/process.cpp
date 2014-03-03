#include "process.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "errors.h"

#include <sys/wait.h>

using namespace sysapi;

child_status::child_status(pid_t pid, int status)
    : pid(pid)
    , status(status)
{}

pid_t sysapi::fork()
{
    pid_t pid = ::fork();
    if (pid < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to fork, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return pid;
}

pid_t sysapi::fork(std::function<void ()> const& child)
{
    pid_t pid = sysapi::fork();

    if (pid == 0)
    {
        try
        {
            child();
        }
        catch (std::exception const& e)
        {
            try
            {
                std::stringstream ss;
                ss << "error: " << e.what();

                std::cerr << ss.str() << std::endl;
            }
            catch (std::exception const&)
            {}
        }
        _Exit(1);
    }

    return pid;
}

void sysapi::waitpid(pid_t pid)
{
    int status;
    pid_t wpid = ::waitpid(pid, &status, 0);
    if (wpid < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to waitpid, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    assert(wpid == pid);
}

boost::optional<child_status> sysapi::reap_child()
{
    int status;
    pid_t wpid = ::waitpid(-1, &status, WNOHANG);
    if (wpid == 0)
        return boost::none;

    if (wpid < 0)
    {
        int err = errno;

        if (err == ECHILD)
            return boost::none;

        std::stringstream ss;
        ss << "unable to waitpid, error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return child_status(wpid, status);
}

void sysapi::execv(std::string const& executable, std::vector<std::string> const& arguments)
{
    std::vector<char*> v;
    v.push_back(const_cast<char*>(executable.c_str()));

    for (std::string const& s : arguments)
    {
        v.push_back(const_cast<char*>(s.c_str()));
    }

    v.push_back(nullptr);

    int exec_result = ::execv(executable.c_str(), v.data());
    assert(exec_result < 0);

    int err = errno;

    std::stringstream ss;
    ss << "unable to exec \"" << executable << "\", error: " << sysapi::errno_to_text(err);

    throw std::runtime_error(ss.str());
}
