#ifndef TASK_DESCRIPTIONS_H
#define TASK_DESCRIPTIONS_H

#include <string>
#include <vector>
#include <map>

#include "task_data.h"
#include "task_description_fwd.h"
#include "run_level.h"

struct task_descriptions
{
    task_descriptions();
    task_descriptions(task_descriptions&& other);
    ~task_descriptions();

    task_description* create_task_description(std::string name, task_data data);
    void create_run_level(std::string name, std::vector<task_description*> requisites);

    std::vector<task_description_sp> const& get_all_tasks() const;
    std::multimap<std::string, task_description*> const& get_task_by_name() const;
    std::map<std::string, run_level> const& get_run_level_by_name() const;

private:
    std::vector<task_description_sp> all_tasks;
    std::multimap<std::string, task_description*> task_by_name;
    std::map<std::string, run_level> run_level_by_name;
};

#endif // TASK_DESCRIPTIONS_H

