#ifndef PSEUDO_IDENTIFIER_VALUE_NODE_H
#define PSEUDO_IDENTIFIER_VALUE_NODE_H

#include "value_node.h"
#include "lexer/identifier_token.h"

struct pseudo_identifier_value_node : value_node
{
    pseudo_identifier_value_node(identifier_token_sp tok);

    value_node_type get_type() const;
    text_range get_range() const;
    identifier_token const& get_token() const;

private:
    identifier_token_sp tok;
};

#endif

