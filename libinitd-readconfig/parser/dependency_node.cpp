#include "dependency_node.h"

dependency_node::dependency_node(identifier_token_sp dependant, std::vector<identifier_token_sp> dependencies)
    : dependant(std::move(dependant))
    , dependencies(std::move(dependencies))
{}

identifier_token const& dependency_node::get_dependant() const
{
    return *dependant;
}

std::vector<identifier_token_sp> const& dependency_node::get_dependencies() const
{
    return dependencies;
}

