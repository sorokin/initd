#ifndef INTEGER_NODE_H
#define INTEGER_NODE_H

#include "value_node.h"
#include "lexer/integer_literal_token.h"

struct integer_node : value_node
{
    integer_node(integer_literal_token_sp tok);

    value_node_type get_type() const;
    text_range get_range() const;
    integer_literal_token const& get_token() const;

private:
    integer_literal_token_sp tok;
};

#endif
