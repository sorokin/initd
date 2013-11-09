#include "run_level_node.h"

run_level_node::run_level_node(identifier_token_sp name, std::vector<identifier_token_sp> dependencies)
    : name(std::move(name))
    , dependencies(std::move(dependencies))
{}

identifier_token const& run_level_node::get_name() const
{
    return *name;
}

std::vector<identifier_token_sp> const& run_level_node::get_dependencies() const
{
    return dependencies;
}

