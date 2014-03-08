#ifndef READ_VALUE_NODE_TASKS_H
#define READ_VALUE_NODE_TASKS_H

#include "read_value_node.h"
#include "task_data.h"

struct pseudo_identifier_value_node;

template <>
struct read_value_node_impl<hostname_task_data>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<hostname_task_data> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<hostname_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<cmd_line>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<cmd_line> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<cmd_line> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<command>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<command> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<command> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<start_stop_task_data>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<start_stop_task_data> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<null_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<control_task_data>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<control_task_data> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<control_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<null_task_data>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<null_task_data> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<null_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

template <>
struct read_value_node_impl<task_data>
{
    static const char* get_type_name();
    static bool can_read(value_node const& node);
    static boost::optional<task_data> read(value_node const& node, error_tag_sink& esink);
    static boost::optional<task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&);
};

#endif
