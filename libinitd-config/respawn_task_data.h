#ifndef RESPAWN_TASK_DATA_H
#define RESPAWN_TASK_DATA_H

#include <boost/optional.hpp>
#include "command.h"

struct respawn_task_data
{
    command start;
    boost::optional<command> stop;
    boost::optional<int> timeout;
};

#endif // RESPAWN_TASK_DATA_H
