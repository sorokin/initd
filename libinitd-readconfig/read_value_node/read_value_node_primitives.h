#ifndef READ_VALUE_NODE_PRIMITIVES_H
#define READ_VALUE_NODE_PRIMITIVES_H

#include "read_value_node.h"

struct pseudo_identifier_value_node;

template <>
struct read_value_node_impl<int>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<int> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<int> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<std::string>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<std::string> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<std::string> value_for_pseudo_identifier(pseudo_identifier_value_node const& node);
};

template <>
struct read_value_node_impl<bool>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<bool> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<bool> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

#endif
