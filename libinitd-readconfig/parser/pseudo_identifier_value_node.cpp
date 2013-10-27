#include "pseudo_identifier_value_node.h"

pseudo_identifier_value_node::pseudo_identifier_value_node(identifier_token_sp tok)
    : tok(std::move(tok))
{}

value_node_type pseudo_identifier_value_node::get_type() const
{
    return value_node_type::pseudo_identifier;
}

text_range pseudo_identifier_value_node::get_range() const
{
    return tok->get_range();
}

identifier_token const& pseudo_identifier_value_node::get_token() const
{
    return *tok;
}
