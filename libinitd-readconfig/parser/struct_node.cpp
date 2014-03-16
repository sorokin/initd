#include "struct_node.h"

#include "lexer/token.h"
#include "property_node.h"

struct_node::struct_node(identifier_token_sp tag, std::vector<property_node_sp> properties)
    : tag(std::move(tag))
    , properties(std::move(properties))
{
    for (property_node_sp const& p : get_properties())
        properties_by_name.insert(std::make_pair(p->get_key().get_text(), p.get()));
}

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

std::multimap<std::string, property_node*> const& struct_node::get_properties_by_name() const
{
    return properties_by_name;
}
