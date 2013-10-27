#include "run_level.h"

#include "task_descriptions.h"

run_level::run_level()
{}

run_level::run_level(std::vector<task_description*> requisites)
    : requisites(std::move(requisites))
{}

run_level run_level::all(task_descriptions const& descr)
{
    run_level ret;

    for (auto const& td : descr.get_all_tasks())
    {
        ret.requisites.push_back(td.get());
    }

    return ret;
}

