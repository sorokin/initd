#include "boolean_node.h"

boolean_node::boolean_node(token_sp tok, bool value)
    : tok(std::move(tok))
    , value(value)
{}

value_node_type boolean_node::get_type() const
{
    return value_node_type::boolean;
}

text_range boolean_node::get_range() const
{
    return tok->get_range();
}

token const& boolean_node::get_token() const
{
    return *tok;
}

bool boolean_node::get_value() const
{
    return value;
}
