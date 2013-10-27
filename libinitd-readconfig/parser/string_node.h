#ifndef STRING_NODE_H
#define STRING_NODE_H

#include "value_node.h"
#include "lexer/string_literal_token.h"

struct string_node : value_node
{
    string_node(string_literal_token_sp tok);

    value_node_type get_type() const;
    text_range get_range() const;
    string_literal_token const& get_token() const;

private:
    string_literal_token_sp tok;
};

#endif
