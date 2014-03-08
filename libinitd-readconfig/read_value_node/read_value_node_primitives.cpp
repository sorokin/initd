#include "read_value_node_primitives.h"

#include "parser/value_node.h"
#include "parser/integer_node.h"
#include "parser/string_node.h"
#include "parser/boolean_node.h"
#include "parser/pseudo_identifier_value_node.h"

const char* read_value_node_impl<int>::get_type_name()
{
    return "int";
}

bool read_value_node_impl<int>::can_read(value_node const& node)
{
    return node.get_type() == value_node_type::integer;
}

boost::optional<int> read_value_node_impl<int>::read(value_node const& node, error_tag_sink& esink)
{
    assert(can_read(node));

    integer_node const& cnode = static_cast<integer_node const&>(node);
    return cnode.get_token().get_value();
}

boost::optional<int> read_value_node_impl<int>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<std::string>::get_type_name()
{
    return "string";
}

bool read_value_node_impl<std::string>::can_read(value_node const& node)
{
    return node.get_type() == value_node_type::string;
}

boost::optional<std::string> read_value_node_impl<std::string>::read(value_node const& node, error_tag_sink& esink)
{
    assert(can_read(node));

    string_node const& cnode = static_cast<string_node const&>(node);
    return cnode.get_token().get_value();
}

boost::optional<std::string> read_value_node_impl<std::string>::value_for_pseudo_identifier(pseudo_identifier_value_node const& node)
{
    return node.get_token().get_text();
}

const char* read_value_node_impl<bool>::get_type_name()
{
    return "bool";
}

bool read_value_node_impl<bool>::can_read(value_node const& node)
{
    return node.get_type() == value_node_type::boolean;
}

boost::optional<bool> read_value_node_impl<bool>::read(value_node const& node, error_tag_sink& esink)
{
    assert(can_read(node));

    boolean_node const& cnode = static_cast<boolean_node const&>(node);
    return cnode.get_value();
}

boost::optional<bool> read_value_node_impl<bool>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

