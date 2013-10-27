#include "task_descriptions.h"

#include "make_unique.h"
#include "task_description.h"

task_descriptions::task_descriptions()
{}

task_descriptions::task_descriptions(task_descriptions&& other)
    : all_tasks(std::move(other.all_tasks))
{}

task_descriptions::~task_descriptions()
{}

task_description* task_descriptions::create_task_description(std::string name, task_data  data)
{
    all_tasks.push_back(make_unique<task_description>(std::move(name), std::move(data)));
    return all_tasks.back().get();
}

void task_descriptions::insert_task_description(task_description_sp d)
{
    all_tasks.push_back(std::move(d));
}

std::vector<task_description_sp> const& task_descriptions::get_all_tasks() const
{
    return all_tasks;
}
