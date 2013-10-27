#include "task.h"
#include "task_handle.h"
#include "make_unique.h"
#include "task_data.h"
#include "task_description.h"

#include "visit_variant.h"

#include "tasks/mount_task.h"
#include "tasks/hostname_task.h"
#include "tasks/null_task.h"

task::task(task_description* descr, task_handle_ptr handle)
    : descr(descr)
    , handle(std::move(handle))
{}

task::~task()
{}

task_ptr create_task(task_description* descr)
{
    task_data const& td = descr->get_data();
    if (mount_task_data const* d = boost::get<mount_task_data>(&td))
        return make_unique<task>(descr, create_mount_task(*d));
    else if (hostname_task_data const* d = boost::get<hostname_task_data>(&td))
        return make_unique<task>(descr, create_hostname_task(*d));
    else if (null_task_data const* d = boost::get<null_task_data>(&td))
        return make_unique<task>(descr, create_null_task(*d));
    else
    {
        assert(false);
        return make_unique<task>(descr, create_null_task(null_task_data{}));
    }
}

void tasks::insert_task(task_ptr&& p)
{
    task_description* descr = p->descr;
    if (is_running(descr))
    {
        assert(false);
        return;
    }

    task_by_description.insert(std::make_pair(descr, std::move(p)));
}

void tasks::erase_task(task_description* descr)
{
    auto i = task_by_description.find(descr);
    if (i != task_by_description.end())
        task_by_description.erase(i);
    else
        assert(false);
}

bool tasks::is_running(task_description* descr) const
{
    return task_by_description.find(descr) != task_by_description.end();
}

boost::select_second_const_range<tasks::task_by_description_t> tasks::all_tasks() const
{
    return task_by_description | boost::adaptors::map_values;
}
