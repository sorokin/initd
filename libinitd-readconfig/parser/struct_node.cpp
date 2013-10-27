#include "struct_node.h"

#include "lexer/token.h"
#include "property_node.h"

struct_node::struct_node(identifier_token_sp tag, std::vector<property_node_sp> properties)
    : tag(std::move(tag))
    , properties(std::move(properties))
{}

value_node_type struct_node::get_type() const
{
    return value_node_type::struct_;
}

text_range struct_node::get_range() const
{
    return tag->get_range();
}

identifier_token const& struct_node::get_tag() const
{
    return *tag;
}

std::vector<property_node_sp> const& struct_node::get_properties() const
{
    return properties;
}
