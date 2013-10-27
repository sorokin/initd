#include "integer_literal_token.h"

#include <boost/range/adaptor/transformed.hpp>
#include "get_raw_pointer.h"

integer_literal_token::integer_literal_token(text_range const& range, int value)
    : range(range)
    , value(value)
{}

text_range integer_literal_token::get_range() const
{
    return range;
}

token_type integer_literal_token::get_type() const
{
    return token_type::integer_literal;
}

int integer_literal_token::get_value() const
{
    return value;
}
