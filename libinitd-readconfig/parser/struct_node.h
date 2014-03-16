#ifndef STRUCT_NODE_H
#define STRUCT_NODE_H

#include <vector>
#include <map>

#include "value_node.h"
#include "property_node_fwd.h"
#include "lexer/token_fwd.h"
#include "lexer/identifier_token.h"

struct struct_node : value_node
{
    struct_node(identifier_token_sp tag, std::vector<property_node_sp> properties);

    value_node_type get_type() const;
    text_range get_range() const;
    identifier_token const& get_tag() const;
    std::vector<property_node_sp> const& get_properties() const;
    std::multimap<std::string, property_node*> const& get_properties_by_name() const;

private:
    identifier_token_sp tag;
    std::vector<property_node_sp> properties;
    std::multimap<std::string, property_node*> properties_by_name;
};

#endif
