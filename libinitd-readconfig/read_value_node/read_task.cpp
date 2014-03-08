#include "read_task.h"
#include "read_value_node_primitives.h"
#include "read_value_node_tasks.h"
#include "extract_property_from_map.h"

#include "parser/task_node.h"

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
