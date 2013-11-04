#include "start_stop_task.h"

#include "task_handle.h"
#include "make_unique.h"
#include "errors.h"
#include "current_directory.h"
#include "process.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace
{
    void run_command(command const& cmd)
    {
        pid_t pid = sysapi::fork([&cmd]() {
            sysapi::set_current_directory(cmd.working_directory);
            sysapi::execv(cmd.cmd.executable, cmd.cmd.arguments);
        });

        sysapi::waitpid(pid);
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

