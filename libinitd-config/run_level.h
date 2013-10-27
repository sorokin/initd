#ifndef RUN_LEVEL_H
#define RUN_LEVEL_H

#include <vector>
#include "task_description_fwd.h"

struct task_descriptions;

struct run_level
{
    run_level();
    run_level(std::vector<task_description*> requisites);
    static run_level all(task_descriptions const&);

    std::vector<task_description*> requisites;
};

#endif // TASK_DESCRIPTION_H

