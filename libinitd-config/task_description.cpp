#include "task_description.h"

task_description::task_description(std::string name, task_data data)
    : name(name)
    , data(data)
{}

std::string const& task_description::get_name() const
{
    return name;
}

task_data const& task_description::get_data() const
{
    return data;
}

void task_description::set_dependencies(std::vector<task_description*> deps)
{
    dependencies = std::move(deps);
}

std::vector<task_description*>& task_description::get_dependencies()
{
    return dependencies;
}

std::vector<task_description*> const& task_description::get_dependencies() const
{
    return dependencies;
}
