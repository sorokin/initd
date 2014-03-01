#include "read_task_data.h"


const char* read_value_node_impl<hostname_task_data>::get_type_name()
{
    return "sethostname";
}

bool read_value_node_impl<hostname_task_data>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<hostname_task_data> read_value_node_impl<hostname_task_data>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    hostname_task_data data;

    boost::optional<std::string> hostname1 = extract_property_from_map<std::string>(cnode, properties_by_name, "hostname", requiredness::required, esink);
    if (!hostname1)
        return boost::none;

    data.hostname = *hostname1;
    data.no_restore = extract_property_from_map<bool>(cnode, properties_by_name, "no_restore", false, esink);

    return data;
}

boost::optional<hostname_task_data> read_value_node_impl<hostname_task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}
