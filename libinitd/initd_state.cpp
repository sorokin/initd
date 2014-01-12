#include "initd_state.h"

#include "tasks/create_async_task_handle.h"
#include "task_description.h"

#include "state_context.h"
#include "task.h"
#include "make_unique.h"

#include <sstream>
#include <stdexcept>

initd_state::initd_state(state_context& ctx, sysapi::epoll& ep, task_descriptions descriptions)
    : ctx(ctx)
    , ep(ep)
    , pending_tasks(0)
{
    auto const& descrs = descriptions.get_all_tasks();
    tasks.resize(descrs.size());

    std::map<task_description*, task*> descr_to_task;

    for (size_t i = 0; i != descrs.size(); ++i)
    {
        tasks[i] = make_unique<task>(create_async_task_handle(*this, [this, i]() {
            tasks[i]->sync(this);

            if (tasks[i]->get_handle()->is_running())
                enqueue_all(tasks[i]->get_dependants());
            else
                enqueue_all(tasks[i]->get_dependencies());

        }, descrs[i]->get_data()));

        descr_to_task.insert(std::make_pair(descrs[i].get(), tasks[i].get()));
    }

    for (size_t i = 0; i != descrs.size(); ++i)
    {
        task_description* descr = descrs[i].get();
        task& my_task = *tasks[i];

        for (task_description* dep : descr->get_dependencies())
        {
            task& dep_task = *descr_to_task.find(dep)->second;
            add_task_dependency(my_task, dep_task);
        }
    }

    for (auto const& name_to_rl : descriptions.get_run_level_by_name())
    {
        std::vector<task*> requisites;
        for (task_description* req : name_to_rl.second.requisites)
            requisites.push_back(descr_to_task.find(req)->second);
        run_levels.insert(std::make_pair(name_to_rl.first, std::move(requisites)));
    }
}

initd_state::~initd_state()
{}

void initd_state::set_run_level(std::string const& run_level_name)
{
    auto i = run_levels.find(run_level_name);
    if (i == run_levels.end())
    {
        std::stringstream ss;
        ss << "run level \"" << run_level_name << "\" is not found";
        throw std::runtime_error(ss.str());
    }

    clear_should_work_flag();
    for (task* d : i->second)
        d->mark_should_work();

    for (task_sp const& tp : tasks)
        tp->sync(this);

    enqueue_all();
}

void initd_state::set_empty_run_level()
{
    clear_should_work_flag();

    for (task_sp const& tp : tasks)
        tp->sync(this);

    enqueue_all();
}

bool initd_state::has_pending_operations() const
{
    return pending_tasks != 0;
}

void initd_state::clear_should_work_flag()
{
    for (task_sp const& tp : tasks)
        tp->clear_should_work();
}

void initd_state::enqueue_all()
{
    for (task_sp const& tp : tasks)
        tp->enqueue_this();
}

void initd_state::enqueue_all(std::vector<task*> const& tts)
{
    for (task* t : tts)
        t->enqueue_this();
}

sysapi::epoll& initd_state::get_epoll()
{
    return ep;
}

state_context& initd_state::get_state_context()
{
    return ctx;
}
