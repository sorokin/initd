#ifndef READ_TASK_DATA_H
#define READ_TASK_DATA_H

#include "task_data.h"
#include "hostname_task_data.h"
#include "null_task_data.h"
#include "read_value_node.h"
#include "parser/task_node.h"
#include "optional_cast.h"

#include <boost/next_prior.hpp>

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

std::multimap<std::string, property_node*> make_properties_map(struct_node const& cnode, error_tag_sink& esink)
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
    static const char* get_type_name()
    {
        return "sethostname";
    }

    static bool can_read(value_node const& node)
    {
        if (node.get_type() != value_node_type::struct_)
            return false;

        struct_node const& cnode = static_cast<struct_node const&>(node);
        return cnode.get_tag().get_text() == get_type_name();
    }

    static boost::optional<hostname_task_data> read(value_node const& node, error_tag_sink& esink)
    {
        assert(node.get_type() == value_node_type::struct_);

        struct_node const& cnode = static_cast<struct_node const&>(node);
        assert(cnode.get_tag().get_text() == get_type_name());

        std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

        hostname_task_data data;

        boost::optional<std::string> hostname1 = extract_property_from_map<std::string>(cnode, properties_by_name, "hostname", esink);
        if (!hostname1)
            return boost::none;

        data.hostname = *hostname1;
        data.no_restore = extract_property_from_map<bool>(cnode, properties_by_name, "no_restore", false, esink);

        return data;
    }

    static boost::optional<hostname_task_data> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
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
            || read_value_node_impl<null_task_data>::can_read(node);
    }

    static boost::optional<task_data> read(value_node const& node, error_tag_sink& esink)
    {
        if (can_read_value_node<hostname_task_data>(node))
            return optional_cast<task_data>(read_value_node<hostname_task_data>(node, esink));
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

boost::optional<task_data> read_task(task_node const& node, error_tag_sink& esink)
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
