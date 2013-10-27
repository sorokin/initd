#ifndef TASK_NODE_H
#define TASK_NODE_H

#include "lexer/identifier_token.h"
#include "value_node_fwd.h"
#include "task_node_fwd.h"

struct task_node
{
    task_node(identifier_token_sp name, value_node_sp value);

    identifier_token const& get_name() const;
    value_node const* get_value() const;

private:
    identifier_token_sp name;
    value_node_sp value;
};

#endif
