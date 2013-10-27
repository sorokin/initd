#include "string_literal_token.h"

#include "get_raw_pointer.h"

string_literal_token::string_literal_token(text_range const& range, std::string value)
    : range(range)
    , value(value)
{}

text_range string_literal_token::get_range() const
{
    return range;
}

token_type string_literal_token::get_type() const
{
    return token_type::string_literal;
}

std::string const& string_literal_token::get_value() const
{
    return value;
}
