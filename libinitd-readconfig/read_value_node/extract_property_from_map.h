#ifndef EXTRACT_PROPERTY_FROM_MAP_H
#define EXTRACT_PROPERTY_FROM_MAP_H

#include <map>
#include <sstream>

#include "lexer/error_tag.h"
#include "parser/struct_node.h"
#include "parser/property_node.h"
#include "parser/pseudo_identifier_value_node.h"

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
    auto i = properties_by_name.find(name);
    if (i == properties_by_name.end())
        return default_value;

    property_node* p = i->second;

    if (!p->get_value())
        return default_value;

    if (p->get_value()->get_type() == value_node_type::pseudo_identifier)
    {
        pseudo_identifier_value_node const& cvalue = static_cast<pseudo_identifier_value_node const&>(*p->get_value());

        std::stringstream ss;
        ss << "excepted value of type \"" << read_value_node_impl<T>::get_type_name() << "\", but identifier found";

        esink.push(error_tag(cvalue.get_token().get_range(), ss.str()));
        if (auto v = read_value_node_impl<T>::value_for_pseudo_identifier(cvalue))
            return *v;

        return default_value;
    }

    if (!can_read_value_node<T>(*p->get_value()))
    {
        std::stringstream ss;
        ss << "property \"" << name
           << "\" has type \"" << read_value_node_impl<T>::get_type_name()
           << "\", but is initialized with \""
           << get_type_by_value_node(*p->get_value()) << "\"";

        esink.push(error_tag(p->get_eq().get_range(), ss.str()));
        return default_value;
    }

    if (auto v = read_value_node<T>(*p->get_value(), esink))
        return *v;

    return default_value;
}

#endif
