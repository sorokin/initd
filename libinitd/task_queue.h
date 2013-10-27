#ifndef TOPOLOGICAL_SORTING_H
#define TOPOLOGICAL_SORTING_H

#include <deque>
#include <functional>

#include "task_description_fwd.h"

struct tasks;
struct task_descriptions;
struct run_level;

struct task_queue_element
{
    bool start;
    task_description* task;
};

bool operator==(task_queue_element const&, task_queue_element const&);
bool operator!=(task_queue_element const&, task_queue_element const&);

typedef std::deque<task_queue_element> task_queue_t;

task_queue_t calculate_task_queue(tasks const& current, task_descriptions const&, run_level const&);

#endif // TOPOLOGICAL_SORTING_H
