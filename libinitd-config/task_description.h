#ifndef TASK_DESCRIPTION_H
#define TASK_DESCRIPTION_H

#include <vector>
#include <memory>
#include <string>

#include "task_description_fwd.h"
#include "task_data.h"

struct task_description
{
    task_description(std::string name, task_data data);

    std::string const& get_name() const;
    task_data const& get_data() const;

    void set_dependencies(std::vector<task_description*>);
    std::vector<task_description*>& get_dependencies();
    std::vector<task_description*> const& get_dependencies() const;

private:
    std::string name;
    task_data data;
    std::vector<task_description*> dependencies;
};

typedef std::unique_ptr<task_description> task_description_sp;

#endif
