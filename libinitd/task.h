#ifndef TASK_H
#define TASK_H

#include <map>
#include <memory>
#include "task_handle_fwd.h"
#include "task_description_fwd.h"

#include <boost/range/adaptor/map.hpp>

struct task
{
    task(task_description* descr, task_handle_ptr handle);
    task(task const&) = delete;
    ~task();

    task_description* descr;
    task_handle_ptr   handle;
};

typedef std::unique_ptr<task> task_ptr;

task_ptr create_task(task_description*);

struct tasks
{
    typedef std::map<task_description*, task_ptr> task_by_description_t;

    void insert_task(task_ptr&& p);
    void erase_task(task_description*);
    bool is_running(task_description*) const;

    boost::select_second_const_range<task_by_description_t> all_tasks() const;

private:
    task_by_description_t task_by_description;
};

#endif // TASK_H
