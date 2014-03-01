#ifndef RESPAWN_TASK_DATA_H
#define RESPAWN_TASK_DATA_H

#include <string>
#include <vector>
#include "start_stop_task_data.h"
#include <boost/optional.hpp>

struct respawn_task_data
{
    command start;
    boost::optional<command> stop;
    boost::optional<int> timeout;
};

#endif // RESPAWN_TASK_DATA_H
