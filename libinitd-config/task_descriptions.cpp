#include "task_descriptions.h"

#include "make_unique.h"
#include "task_description.h"
#include <utility>

task_descriptions::task_descriptions()
{}

task_descriptions::task_descriptions(task_descriptions&& other)
    : all_tasks(std::move(other.all_tasks))
    , task_by_name(std::move(other.task_by_name))
    , run_level_by_name(std::move(other.run_level_by_name))
{}

task_descriptions::~task_descriptions()
{}

task_description* task_descriptions::create_task_description(std::string name, task_data data)
{
    all_tasks.push_back(make_unique<task_description>(name, std::move(data)));
    task_by_name.insert(std::make_pair(std::move(name), all_tasks.back().get()));
    return all_tasks.back().get();
}

void task_descriptions::create_run_level(std::string name, std::vector<task_description*> requisites)
{
    run_level_by_name.insert(std::make_pair(std::move(name), std::move(requisites)));
}

std::vector<task_description_sp> const& task_descriptions::get_all_tasks() const
{
    return all_tasks;
}

std::multimap<std::string, task_description*> const& task_descriptions::get_task_by_name() const
{
    return task_by_name;
}

std::map<std::string, run_level> const& task_descriptions::get_run_level_by_name() const
{
    return run_level_by_name;
}
