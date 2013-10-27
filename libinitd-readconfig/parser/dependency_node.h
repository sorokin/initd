#ifndef DEPENDENCY_NODE_H
#define DEPENDENCY_NODE_H

#include "lexer/identifier_token.h"
#include <vector>
#include <memory>

struct dependency_node
{
    dependency_node(identifier_token_sp dependant, std::vector<identifier_token_sp> dependencies);

    identifier_token const& get_dependant() const;
    std::vector<identifier_token_sp> const& get_dependencies() const;

private:
    identifier_token_sp dependant;
    std::vector<identifier_token_sp> dependencies;
};

typedef std::unique_ptr<dependency_node> dependency_node_sp;

#endif
