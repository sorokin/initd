#ifndef COMMAND_H
#define COMMAND_H

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

#endif
