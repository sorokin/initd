#ifndef BOOLEAN_NODE_H
#define BOOLEAN_NODE_H

#include "value_node.h"
#include "lexer/token.h"

struct boolean_node : value_node
{
    boolean_node(token_sp tok, bool value);

    value_node_type get_type() const;
    text_range get_range() const;
    token const& get_token() const;
    bool get_value() const;

private:
    token_sp tok;
    bool value;
};

#endif
