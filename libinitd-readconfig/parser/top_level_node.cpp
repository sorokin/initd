#include "top_level_node.h"

#include "lexer/token.h"
#include "task_node.h"
#include "value_node.h"

top_level_node::top_level_node(std::vector<task_node_sp> tasks, std::vector<dependency_node_sp> dependencies_decls)
    : tasks(std::move(tasks))
    , dependencies_decls(std::move(dependencies_decls))
{}

std::vector<task_node_sp> const& top_level_node::get_tasks() const
{
    return tasks;
}

std::vector<dependency_node_sp> const& top_level_node::get_dependencies_decls() const
{
    return dependencies_decls;
}
