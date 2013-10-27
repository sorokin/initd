#ifndef TOP_LEVEL_NODE_H
#define TOP_LEVEL_NODE_H

#include <vector>
#include "task_node_fwd.h"
#include "dependency_node.h"
#include "top_level_node_fwd.h"

struct top_level_node
{
    top_level_node(std::vector<task_node_sp> tasks, std::vector<dependency_node_sp> dependencies_decls);

    std::vector<task_node_sp> const& get_tasks() const;
    std::vector<dependency_node_sp> const& get_dependencies_decls() const;

private:
    std::vector<task_node_sp> tasks;
    std::vector<dependency_node_sp> dependencies_decls;
};

#endif
