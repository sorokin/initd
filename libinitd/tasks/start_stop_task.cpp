#include "start_stop_task.h"

#include "task_handle.h"
#include "make_unique.h"
#include "errors.h"
#include "current_directory.h"

#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace
{
    void run_command(command const& cmd)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to fork, error: " << sysapi::errno_to_text(err);

            throw std::runtime_error(ss.str());
        }

        if (pid == 0)
        {
            try
            {
                std::vector<char*> v;
                v.push_back(const_cast<char*>(cmd.cmd.executable.c_str()));

                for (std::string const& s : cmd.cmd.arguments)
                {
                    v.push_back(const_cast<char*>(s.c_str()));
                }

                v.push_back(nullptr);

                sysapi::set_current_directory(cmd.working_directory);

                int exec_result = execv(cmd.cmd.executable.c_str(), v.data());
                assert(exec_result < 0);

                int err = errno;

                std::stringstream ss;
                ss << "unable to exec, error: " << sysapi::errno_to_text(err);

                std::cerr << ss.str() << std::endl;
                _Exit(1);
            }
            catch (std::exception const& e)
            {
                std::stringstream ss;
                ss << "error: " << e.what();

                std::cerr << ss.str() << std::endl;
                _Exit(1);
            }
        }

        int status;
        pid_t wpid = waitpid(pid, &status, 0);
        if (wpid < 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to waitpid, error: " << sysapi::errno_to_text(err);

            throw std::runtime_error(ss.str());
        }

        assert(wpid == pid);
    }

    struct start_stop_task_handle : task_handle
    {
        start_stop_task_handle(start_stop_task_data const& data)
            : stop_cmd(data.stop)
        {
            run_command(data.start);
        }

        ~start_stop_task_handle()
        {
            try
            {
                run_command(stop_cmd);
            }
            catch (std::exception const& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        std::string status_message() const
        {
            return "start-stop task";
        }

    private:
        command stop_cmd;
    };
}

task_handle_ptr create_start_stop_task(start_stop_task_data const& data)
{
    return make_unique<start_stop_task_handle>(data);
}

