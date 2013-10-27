#define BOOST_TEST_MODULE initd_task_queue
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <set>

#include <boost/test/unit_test.hpp>

#include "task_description.h"
#include "task_descriptions.h"
#include "run_level.h"
#include "task.h"
#include "task_queue.h"
#include "task_handle.h"
#include "make_unique.h"
#include "tasks/null_task.h"

namespace
{
    void remove_description_dfs(std::set<task_description*>& s, task_description* d)
    {
        auto i = s.find(d);
        if (i == s.end())
            return;

        s.erase(i);
        for (task_description* dependency : d->get_dependencies())
            remove_description_dfs(s, dependency);
    }

    void check_queue(tasks const& current, run_level const& desired, task_queue_t const& queue)
    {
        std::set<task_description*> working;

        for (auto const& p : current.all_tasks())
        {
            task_description* d = p->descr;
            assert(working.find(d) == working.end());
            working.insert(d);
        }

        for (task_queue_element const& e : queue)
        {
            if (e.start)
            {
                BOOST_VERIFY(working.find(e.task) == working.end() && "task already working");
                for (task_description* dependency : e.task->get_dependencies())
                    BOOST_VERIFY(working.find(dependency) != working.end() && "dependency is not working");

                working.insert(e.task);
            }
            else
            {
                BOOST_VERIFY(working.find(e.task) != working.end() && "task is not working");
                working.erase(e.task);

                // not optimal
                for (task_description* task : working)
                    for (task_description* dependency : task->get_dependencies())
                        BOOST_VERIFY(working.find(dependency) != working.end() && "dependency not working");
            }
        }

        for (task_description* task : desired.requisites)
        {
            BOOST_CHECK(working.find(task) != working.end());
        }

        for (task_description* task : desired.requisites)
        {
            remove_description_dfs(working, task);
        }

        BOOST_CHECK(working.empty());
    }
}

BOOST_AUTO_TEST_CASE(one_task)
{
    task_descriptions descr;
    task_description* a = descr.create_task_description("test", null_task_data());

    tasks current;
    run_level rlevel{{a}};

    auto queue = calculate_task_queue(current, descr, rlevel);
    check_queue(current, rlevel, queue);

    BOOST_CHECK(queue == (task_queue_t{{true, a}}));
}

BOOST_AUTO_TEST_CASE(two_tasks)
{
    task_descriptions descr;
    task_description* a = descr.create_task_description("test", null_task_data());
    task_description* b = descr.create_task_description("test", null_task_data());
    b->set_dependencies({a});

    tasks current;
    run_level rlevel{{b}};

    auto queue = calculate_task_queue(current, descr, rlevel);
    check_queue(current, rlevel, queue);
    BOOST_CHECK(queue == (task_queue_t{{true, a}, {true, b}}));
}

BOOST_AUTO_TEST_CASE(ex)
{
    task_descriptions descr;
    task_description* a = descr.create_task_description("test", null_task_data());
    task_description* b = descr.create_task_description("test", null_task_data());
    task_description* c = descr.create_task_description("test", null_task_data());
    task_description* d = descr.create_task_description("test", null_task_data());
    task_description* e = descr.create_task_description("test", null_task_data());
    c->set_dependencies({a, b});
    d->set_dependencies({c});
    e->set_dependencies({c});

    tasks current;
    run_level rlevel{{d, e}};

    auto queue = calculate_task_queue(current, descr, rlevel);
    check_queue(current, rlevel, queue);
}

BOOST_AUTO_TEST_CASE(ex_partial_switch)
{
    task_descriptions descr;
    task_description* a = descr.create_task_description("test", null_task_data());
    task_description* b = descr.create_task_description("test", null_task_data());
    task_description* c = descr.create_task_description("test", null_task_data());
    task_description* d = descr.create_task_description("test", null_task_data());
    task_description* e = descr.create_task_description("test", null_task_data());
    c->set_dependencies({a, b});
    d->set_dependencies({c});
    e->set_dependencies({c});

    tasks current;
    current.insert_task(create_task(a));
    current.insert_task(create_task(b));
    current.insert_task(create_task(c));
    current.insert_task(create_task(d));

    run_level rlevel{{e}};

    auto queue = calculate_task_queue(current, descr, rlevel);
    check_queue(current, rlevel, queue);
    BOOST_CHECK(queue == (task_queue_t{{false, d}, {true, e}}));
}
