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

inline std::multimap<std::string, property_node*> make_properties_map(struct_node const& cnode, error_tag_sink& esink)
{
    std::multimap<std::string, property_node*> properties_by_name;
    for (property_node_sp const& p : cnode.get_properties())
        properties_by_name.insert(std::make_pair(p->get_key().get_text(), p.get()));

    report_duplicates(properties_by_name, [&](property_node* p) { esink.push(error_tag(p->get_key().get_range(), "duplicate property declaration")); });

    return properties_by_name;
}

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
    static const char* get_type_name()
    {
        return "cmd_line";
    }

    static bool can_read(value_node const& node)
    {
        return node.get_type() == value_node_type::string;
    }

    static boost::optional<cmd_line> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::string);

        string_node const& cnode = static_cast<string_node const&>(node);
        std::string const& value = cnode.get_token().get_value();

        std::vector<std::string> split;
        std::string::const_iterator i = value.cbegin();
        for (;;)
        {
            i = std::find_if(i, value.cend(), [](char c) { return !is_ascii_whitespace(c); });
            if (i == value.cend())
                break;

            std::string arg;

            for (;;)
            {
                char c = *i;
                if (c == '\'')
                {
                    ++i;
                    auto j = std::find_if(i, value.cend(), [](char c) { return c == '\''; });
                    if (j == value.cend())
                    {
                        esink.push(error_tag(node.get_range(), "unmatched \'"));
                        return boost::none;
                    }

                    arg.insert(arg.end(), i, j);
                    ++j;
                    i = j;
                }
                else if (c == '\"')
                {
                    ++i;
                    auto j = std::find_if(i, value.cend(), [](char c) { return c == '\"'; });
                    if (j == value.cend())
                    {
                        esink.push(error_tag(node.get_range(), "unmatched \""));
                        return boost::none;
                    }

                    arg.insert(arg.end(), i, j);
                    ++j;
                    i = j;
                }
                else if (is_ascii_whitespace(c))
                    break;
                else
                {
                    arg += c;
                    ++i;
                }
            }

            split.push_back(arg);
        }

        if (split.empty())
        {
            esink.push(error_tag(node.get_range(), "empty command line"));
            return boost::none;
        }

        cmd_line data;
        data.executable = split[0];
        data.arguments.assign(split.begin() + 1, split.end());

        return data;
    }

    static boost::optional<cmd_line> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<command>
{
    static const char* get_type_name()
    {
        return "command";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<command> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        boost::optional<cmd_line> cmd = extract_property_from_map<cmd_line>(cnode, properties_by_name, "cmd", requiredness::required, esink);
        boost::optional<std::string> working_directory = extract_property_from_map<std::string>(cnode, properties_by_name, "working_directory", requiredness::required, esink);
        if (!cmd || !working_directory)
            return boost::none;

        command data;

        data.cmd = *cmd;
        data.working_directory = *working_directory;

        return data;
    }

    static boost::optional<command> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<start_stop_task_data>
{
    static const char* get_type_name()
    {
        return "start_stop";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<start_stop_task_data> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        start_stop_task_data data;

        boost::optional<command> start_cmd = extract_property_from_map<command>(cnode, properties_by_name, "start", requiredness::required, esink);
        boost::optional<command> stop_cmd = extract_property_from_map<command>(cnode, properties_by_name, "stop", requiredness::required, esink);
        if (!start_cmd || !stop_cmd)
            return boost::none;

        data.start = *start_cmd;
        data.stop  = *stop_cmd;

        return data;
    }

    static boost::optional<null_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<respawn_task_data>
{
    static const char* get_type_name()
    {
        return "respawn";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<respawn_task_data> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        respawn_task_data data;

        boost::optional<command> start_cmd = extract_property_from_map<command>(cnode, properties_by_name, "start", requiredness::required, esink);
        boost::optional<command> stop_cmd = extract_property_from_map<command>(cnode, properties_by_name, "stop", requiredness::not_required, esink);
        boost::optional<int> timeout = extract_property_from_map<int>(cnode, properties_by_name, "timeout", 30, esink);

        if (!start_cmd)
            return boost::none;

        data.start = *start_cmd;

        if (stop_cmd)
            data.stop = *stop_cmd;

        if (timeout)
            data.timeout = *timeout;

        return data;
    }

    static boost::optional<null_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<control_task_data>
{
    static const char* get_type_name()
    {
        return "control";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<control_task_data> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        control_task_data data;

        boost::optional<std::string> uds_filename = extract_property_from_map<std::string>(cnode, properties_by_name, "uds_filename", requiredness::required, esink);
        if (!uds_filename)
            return boost::none;
        data.uds_filename = *uds_filename;

        return data;
    }

    static boost::optional<control_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<null_task_data>
{
    static const char* get_type_name()
    {
        return "null";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<null_task_data> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        return null_task_data();
    }

    static boost::optional<null_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};


template <>
struct read_value_node_impl<task_data>
{
    static const char* get_type_name()
    {
        return "task";
    }

    static bool can_read(value_node const& node)
    {
        return read_value_node_impl<hostname_task_data>::can_read(node)
            || read_value_node_impl<start_stop_task_data>::can_read(node)
            || read_value_node_impl<respawn_task_data>::can_read(node)
            || read_value_node_impl<control_task_data>::can_read(node)
            || read_value_node_impl<null_task_data>::can_read(node);
    }

    static boost::optional<task_data> read(value_node const& node, error_tag_sink& esink)
    {
        if (can_read_value_node<hostname_task_data>(node))
            return optional_cast<task_data>(read_value_node<hostname_task_data>(node, esink));
        else if (can_read_value_node<start_stop_task_data>(node))
            return optional_cast<task_data>(read_value_node<start_stop_task_data>(node, esink));
        else if (can_read_value_node<respawn_task_data>(node))
            return optional_cast<task_data>(read_value_node<respawn_task_data>(node, esink));
        else if (can_read_value_node<control_task_data>(node))
            return optional_cast<task_data>(read_value_node<control_task_data>(node, esink));
        else if (can_read_value_node<null_task_data>(node))
            return optional_cast<task_data>(read_value_node<null_task_data>(node, esink));
        else
        {
            assert(false);
            return boost::none;
        }
    }

    static boost::optional<task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

inline boost::optional<task_data> read_task(task_node const& node, error_tag_sink& esink)
{
    if (!node.get_value())
        return boost::none;

    if (node.get_value()->get_type() == value_node_type::pseudo_identifier)
    {
        pseudo_identifier_value_node const& cvalue = static_cast<pseudo_identifier_value_node const&>(*node.get_value());

        std::stringstream ss;
        ss << "excepted value of type \"" << read_value_node_impl<task_data>::get_type_name() << "\", but identifier found";

        esink.push(error_tag(cvalue.get_token().get_range(), ss.str()));
        return read_value_node_impl<task_data>::value_for_pseudo_identifier(cvalue);
    }

    if (!can_read_value_node<task_data>(*node.get_value()))
    {
        std::stringstream ss;
        ss << "\"" << get_type_by_value_node(*node.get_value()) << "\" is not a valid type for task";

        esink.push(error_tag(node.get_value()->get_range(), ss.str()));
        return boost::none;
    }

    return read_value_node<task_data>(*node.get_value(), esink);
}

#endif
