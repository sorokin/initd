#include "read_task_data.h"

std::multimap<std::string, property_node*> make_properties_map(struct_node const& cnode, error_tag_sink& esink)
{
    std::multimap<std::string, property_node*> properties_by_name;
    for (property_node_sp const& p : cnode.get_properties())
        properties_by_name.insert(std::make_pair(p->get_key().get_text(), p.get()));

    report_duplicates(properties_by_name, [&](property_node* p) { esink.push(error_tag(p->get_key().get_range(), "duplicate property declaration")); });

    return properties_by_name;
}

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
