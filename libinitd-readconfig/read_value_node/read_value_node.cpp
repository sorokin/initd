#include "read_value_node.h"

std::string get_type_by_value_node(value_node const& node)
{
    switch (node.get_type())
    {
    case value_node_type::integer:
        return "int";
    case value_node_type::string:
        return "string";
    case value_node_type::boolean:
        return "bool";
    case value_node_type::pseudo_identifier:
        assert(false);
        return "<pseudo identifier>";
    case value_node_type::struct_:
        {
            struct_node const& cnode = static_cast<struct_node const&>(node);
            return cnode.get_tag().get_text();
        }
    default:
        assert(false);
        return "<invalid value_node_type>";
    }
}
