#include "simple_token.h"

simple_token::simple_token(text_range const& range, token_type type)
    : range(range)
    , type(type)
{}

text_range simple_token::get_range() const
{
    return range;
}

token_type simple_token::get_type() const
{
    return type;
}
