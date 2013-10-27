#include "task_queue.h"

#include "task.h"
#include "task_descriptions.h"
#include "task_description.h"
#include "run_level.h"

#include <assert.h>
#include <map>
#include <set>

namespace
{
    struct reversed_graph
    {
        struct vertex_data
        {
            bool currently_working = false;
            bool should_work = false;

            std::vector<task_description*> edges;
        };

        std::map<task_description*, vertex_data> vertices;
        std::vector<task_description*> tasks_with_no_dependencies;
    };

    void build_reversed_graph(reversed_graph& output, task_descriptions const& descr)
    {
        for (auto const& p : descr.get_all_tasks())
        {
            task_description* task = p.get();
            output.vertices.insert(std::make_pair(task, reversed_graph::vertex_data{}));
        }

        for (auto const& p : descr.get_all_tasks())
        {
            task_description* task = p.get();
            for (task_description* dependency : task->get_dependencies())
            {
                output.vertices.find(dependency)->second.edges.push_back(task);
            }

            if (task->get_dependencies().empty())
                output.tasks_with_no_dependencies.push_back(task);
        }
    }

    void mark_currently_working(reversed_graph& output, tasks const& current)
    {
        for (auto const& p : current.all_tasks())
        {
            task* ptask = p.get();
            output.vertices.find(ptask->descr)->second.currently_working = true;
        }
    }

    void mark_should_work(reversed_graph& output, task_description* d)
    {
        output.vertices.find(d)->second.should_work = true;

        for (task_description* dependency : d->get_dependencies())
            mark_should_work(output, dependency);
    }

    void mark_should_work(reversed_graph& output, run_level const& desired)
    {
        for (task_description* p : desired.requisites)
            mark_should_work(output, p);
    }

    void remove_unnecessary_dfs(task_queue_t& output, reversed_graph const& g, task_description* current, std::set<task_description*>& visited)
    {
        if (visited.find(current) != visited.end())
            return;

        visited.insert(current);

        reversed_graph::vertex_data const& v = g.vertices.find(current)->second;
        for (task_description* dependent : v.edges)
        {
            remove_unnecessary_dfs(output, g, dependent, visited);
        }

        if (v.currently_working && !v.should_work)
            output.push_back({false, current});
    }

    void remove_unnecessary(task_queue_t& output, reversed_graph const& g)
    {
        std::set<task_description*> visited;

        //for (task_description* root : g.tasks_with_no_dependencies)
        for (std::vector<task_description*>::const_iterator i = g.tasks_with_no_dependencies.begin(); i != g.tasks_with_no_dependencies.end(); ++i)
        {
            task_description* root = *i;
            remove_unnecessary_dfs(output, g, root, visited);
        }
    }

    void add_necessary_dfs(task_queue_t& output, reversed_graph const& g, task_description* current, std::set<task_description*>& visited)
    {
        if (visited.find(current) != visited.end())
            return;

        visited.insert(current);

        for (task_description* dependency : current->get_dependencies())
        {
            add_necessary_dfs(output, g, dependency, visited);
        }

        reversed_graph::vertex_data const& v = g.vertices.find(current)->second;
        assert(v.should_work);

        if (!v.currently_working)
            output.push_back({true, current});
    }

    void add_necessary(task_queue_t& output, reversed_graph const& g, run_level const& desired)
    {
        std::set<task_description*> visited;

        for (task_description* root : desired.requisites)
        {
            add_necessary_dfs(output, g, root, visited);
        }
    }
}

bool operator==(task_queue_element const& a, task_queue_element const& b)
{
    return a.start == b.start
        && a.task == b.task;
}

bool operator!=(task_queue_element const& a, task_queue_element const& b)
{
    return !(a == b);
}

task_queue_t calculate_task_queue(tasks const& current, task_descriptions const& descr, run_level const& desired)
{
    reversed_graph g;
    build_reversed_graph(g, descr);
    mark_currently_working(g, current);
    mark_should_work(g, desired);

    task_queue_t res;
    remove_unnecessary(res, g);
    add_necessary(res, g, desired);
    return res;
}
