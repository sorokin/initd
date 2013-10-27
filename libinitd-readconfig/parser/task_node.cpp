#include "task_node.h"

#include "lexer/token.h"
#include "value_node.h"

task_node::task_node(identifier_token_sp name, value_node_sp value)
    : name(std::move(name))
    , value(std::move(value))
{}

identifier_token const& task_node::get_name() const
{
    return *name;
}

value_node const* task_node::get_value() const
{
    return value.get();
}
