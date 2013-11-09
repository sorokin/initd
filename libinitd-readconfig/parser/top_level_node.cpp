#include "top_level_node.h"

#include "lexer/token.h"
#include "task_node.h"
#include "run_level_node.h"
#include "value_node.h"

top_level_node::top_level_node(std::vector<task_node_sp> tasks,
                               std::vector<dependency_node_sp> dependencies_decls,
                               std::vector<run_level_node_sp> run_levels)
    : tasks(std::move(tasks))
    , dependencies_decls(std::move(dependencies_decls))
    , run_levels(std::move(run_levels))
{}

top_level_node::~top_level_node()
{}

std::vector<task_node_sp> const& top_level_node::get_tasks() const
{
    return tasks;
}

std::vector<dependency_node_sp> const& top_level_node::get_dependencies_decls() const
{
    return dependencies_decls;
}

std::vector<run_level_node_sp> const& top_level_node::get_run_levels() const
{
    return run_levels;
}
