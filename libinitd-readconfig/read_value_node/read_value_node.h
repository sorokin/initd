#ifndef READ_VALUE_NODE_H
#define READ_VALUE_NODE_H

#include "lexer/error_tag.h"
#include "parser/value_node_fwd.h"
#include "parser/integer_node.h"
#include "parser/string_node.h"
#include "parser/boolean_node.h"
#include "parser/pseudo_identifier_value_node.h"
#include "parser/struct_node.h"
#include "parser/property_node.h"

#include <sstream>
#include <string>
#include <map>

#include <boost/optional.hpp>

template <typename T>
struct read_value_node_impl;

template <typename T>
boost::optional<T> read_value_node(value_node const& node, error_tag_sink& esink)
{
    return read_value_node_impl<T>::read(node, esink);
}

template <typename T>
bool can_read_value_node(value_node const& node)
{
    return read_value_node_impl<T>::can_read(node);
}

std::string get_type_by_value_node(value_node const& node);

template <>
struct read_value_node_impl<int>
{
    static const char* get_type_name()
    {
        return "int";
    }

    static bool can_read(value_node const& node)
    {
        return node.get_type() == value_node_type::integer;
    }

    static boost::optional<int> read(value_node const& node, error_tag_sink& esink)
    {
        assert(can_read(node));

        integer_node const& cnode = static_cast<integer_node const&>(node);
        return cnode.get_token().get_value();
    }

    static boost::optional<int> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <>
struct read_value_node_impl<std::string>
{
    static const char* get_type_name()
    {
        return "string";
    }

    static bool can_read(value_node const& node)
    {
        return node.get_type() == value_node_type::string;
    }

    static boost::optional<std::string> read(value_node const& node, error_tag_sink& esink)
    {
        assert(can_read(node));

        string_node const& cnode = static_cast<string_node const&>(node);
        return cnode.get_token().get_value();
    }

    static boost::optional<std::string> value_for_pseudo_identifier(pseudo_identifier_value_node const& node)
    {
        return node.get_token().get_text();
    }
};

template <>
struct read_value_node_impl<bool>
{
    static const char* get_type_name()
    {
        return "bool";
    }

    static bool can_read(value_node const& node)
    {
        return node.get_type() == value_node_type::boolean;
    }

    static boost::optional<bool> read(value_node const& node, error_tag_sink& esink)
    {
        assert(can_read(node));

        boolean_node const& cnode = static_cast<boolean_node const&>(node);
        return cnode.get_value();
    }

    static boost::optional<bool> value_for_pseudo_identifier(pseudo_identifier_value_node const&)
    {
        return boost::none;
    }
};

template <typename T>
boost::optional<T> extract_property_from_map(struct_node const& struct_,
                                             std::multimap<std::string, property_node*> const& properties_by_name,
                                             std::string const& name,
                                             error_tag_sink& esink)
{
    auto i = properties_by_name.find(name);
    if (i == properties_by_name.end())
    {
        std::stringstream ss;
        ss << "property \"" << name << "\" is not found in struct \"" << struct_.get_tag().get_text() << "\"";

        esink.push(error_tag(struct_.get_tag().get_range(), ss.str()));
        return boost::none;
    }

    property_node* p = i->second;

    if (!p->get_value())
        return boost::none;

    if (p->get_value()->get_type() == value_node_type::pseudo_identifier)
    {
        pseudo_identifier_value_node const& cvalue = static_cast<pseudo_identifier_value_node const&>(*p->get_value());

        std::stringstream ss;
        ss << "excepted value of type \"" << read_value_node_impl<T>::get_type_name() << "\", but identifier found";

        esink.push(error_tag(cvalue.get_token().get_range(), ss.str()));
        return read_value_node_impl<T>::value_for_pseudo_identifier(cvalue);
    }

    if (!can_read_value_node<T>(*p->get_value()))
    {
        std::stringstream ss;
        ss << "property \"" << name
           << "\" has type \"" << read_value_node_impl<T>::get_type_name()
           << "\", but is initialized with \""
           << get_type_by_value_node(*p->get_value()) << "\"";

        esink.push(error_tag(p->get_eq().get_range(), ss.str()));
        return boost::none;
    }

    return read_value_node<T>(*p->get_value(), esink);
}

template <typename T>
T extract_property_from_map(struct_node const& cnode,
                            std::multimap<std::string, property_node*> const& properties_by_name,
                            std::string const& name,
                            T default_value,
                            error_tag_sink& esink)
{
    boost::optional<T> t = extract_property_from_map<T>(cnode, properties_by_name, name, esink);
    if (!t)
        return default_value;

    return *t;
}


#endif
