#ifndef START_STOP_TASK_DATA_H
#define START_STOP_TASK_DATA_H

#include <string>
#include <vector>

struct command
{
    command(std::string const& executable);
    command(std::string const& executable, std::string const& param0);

    std::string executable;
    std::vector<std::string> params;
};

struct start_stop_task_data
{
    command start;
    command stop;
};

#endif // START_STOP_TASK_DATA_H
