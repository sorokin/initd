#ifndef RUN_LEVEL_NODE_H
#define RUN_LEVEL_NODE_H

#include "run_level_node_fwd.h"
#include "lexer/token_fwd.h"
#include "lexer/identifier_token.h"

#include <vector>

struct run_level_node
{
    run_level_node(identifier_token_sp name, std::vector<identifier_token_sp> dependencies);

    identifier_token const& get_name() const;
    std::vector<identifier_token_sp> const& get_dependencies() const;

private:
    identifier_token_sp name;
    std::vector<identifier_token_sp> dependencies;
};

#endif
