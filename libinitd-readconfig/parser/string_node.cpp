#include "string_node.h"

string_node::string_node(string_literal_token_sp tok)
    : tok(std::move(tok))
{}

value_node_type string_node::get_type() const
{
    return value_node_type::string;
}

text_range string_node::get_range() const
{
    return tok->get_range();
}

string_literal_token const& string_node::get_token() const
{
    return *tok;
}
