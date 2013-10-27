#include "property_node.h"

#include "value_node.h"
#include "lexer/token.h"
#include "lexer/text_range.h"

property_node::property_node(identifier_token_sp key, token_sp eq, value_node_sp value)
    : key(std::move(key))
    , eq(std::move(eq))
    , value(std::move(value))
{}

identifier_token const& property_node::get_key() const
{
    return *key;
}

token const& property_node::get_eq() const
{
    return *eq;
}

value_node const* property_node::get_value() const
{
    return value.get();
}
