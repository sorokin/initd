#ifndef READ_VALUE_NODE_H
#define READ_VALUE_NODE_H

#include <string>

#include <boost/optional.hpp>

struct value_node;
struct error_tag_sink;

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

#endif
