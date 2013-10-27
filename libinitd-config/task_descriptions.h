#ifndef TASK_DESCRIPTIONS_H
#define TASK_DESCRIPTIONS_H

#include <string>
#include <vector>
#include "task_data.h"
#include "task_description_fwd.h"

struct task_descriptions
{
    task_descriptions();
    task_descriptions(task_descriptions&& other);
    ~task_descriptions();

    task_description* create_task_description(std::string name, task_data data);
    void insert_task_description(task_description_sp);

    std::vector<task_description_sp> const& get_all_tasks() const;

private:
    std::vector<task_description_sp> all_tasks;
};

#endif // TASK_DESCRIPTIONS_H

