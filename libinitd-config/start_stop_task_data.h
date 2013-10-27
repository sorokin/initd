#ifndef START_STOP_TASK_DATA_H
#define START_STOP_TASK_DATA_H

#include <string>
#include <vector>

struct cmd_line
{
    std::string executable;
    std::vector<std::string> arguments;
};

struct command
{
    command();
    command(cmd_line const& cmd);

    cmd_line cmd;
    std::string working_directory;
};

struct start_stop_task_data
{
    command start;
    command stop;
};

#endif // START_STOP_TASK_DATA_H
