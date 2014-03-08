#ifndef READ_TASK_DATA_H
#define READ_TASK_DATA_H

#include <algorithm>

#include <boost/next_prior.hpp>

#include "task_data.h"
#include "hostname_task_data.h"
#include "control_task_data.h"
#include "null_task_data.h"
#include "read_value_node.h"
#include "parser/task_node.h"
#include "lexer/lexer.h"
#include "optional_cast.h"

struct top_level_node;

template <typename K, typename V, typename F>
void report_duplicates(std::multimap<K, V> const& m, F f)
{
    if (m.empty())
        return;

    typedef typename std::multimap<K, V>::const_iterator const_iterator;

    bool last_reported = false;

    for (const_iterator i = boost::next(m.begin()); i != m.end(); ++i)
    {
        const_iterator pi = boost::prior(i);
        if (pi->first == i->first)
        {
            if (!last_reported)
                f(pi->second);

            f(i->second);
            last_reported = true;
        }
    }
}

std::multimap<std::string, property_node*> make_properties_map(struct_node const& cnode, error_tag_sink& esink);

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

boost::optional<task_data> read_task(task_node const& node, error_tag_sink& esink);

#endif
