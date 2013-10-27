#ifndef PROPERTY_NODE_H
#define PROPERTY_NODE_H

#include "property_node_fwd.h"
#include "lexer/token_fwd.h"
#include "lexer/identifier_token.h"
#include "value_node_fwd.h"

#include <string>

struct property_node
{
    property_node(identifier_token_sp key, token_sp eq, value_node_sp value);

    identifier_token const& get_key() const;
    token            const& get_eq() const;
    value_node       const* get_value() const;

private:
    identifier_token_sp key;
    token_sp eq;
    value_node_sp value;
};

#endif
