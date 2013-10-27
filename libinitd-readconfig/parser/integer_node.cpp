#include "integer_node.h"

integer_node::integer_node(integer_literal_token_sp tok)
    : tok(std::move(tok))
{}

value_node_type integer_node::get_type() const
{
    return value_node_type::integer;
}

text_range integer_node::get_range() const
{
    return tok->get_range();
}

integer_literal_token const& integer_node::get_token() const
{
    return *tok;
}
