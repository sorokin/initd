#ifndef TOP_LEVEL_NODE_H
#define TOP_LEVEL_NODE_H

#include <vector>
#include "task_node_fwd.h"
#include "dependency_node.h"
#include "run_level_node_fwd.h"
#include "top_level_node_fwd.h"

struct top_level_node
{
    top_level_node(std::vector<task_node_sp> tasks,
                   std::vector<dependency_node_sp> dependencies_decls,
                   std::vector<run_level_node_sp> run_levels);
    ~top_level_node();

    std::vector<task_node_sp> const& get_tasks() const;
    std::vector<dependency_node_sp> const& get_dependencies_decls() const;
    std::vector<run_level_node_sp> const& get_run_levels() const;

private:
    std::vector<task_node_sp> tasks;
    std::vector<dependency_node_sp> dependencies_decls;
    std::vector<run_level_node_sp> run_levels;
};

#endif
