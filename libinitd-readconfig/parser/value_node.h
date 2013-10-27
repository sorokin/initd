#ifndef VALUE_NODE_H
#define VALUE_NODE_H

#include "value_node_fwd.h"
#include "lexer/text_range.h"

enum class value_node_type
{
    integer,
    string,
    boolean,
    pseudo_identifier,
    struct_,
};

struct value_node
{
    virtual value_node_type get_type() const = 0;
    virtual text_range get_range() const = 0;
    virtual ~value_node();
};

#endif
