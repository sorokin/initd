#include "identifier_token.h"

identifier_token::identifier_token(text_range const& range, std::string text)
    : range(range)
    , text(std::move(text))
{}

text_range identifier_token::get_range() const
{
    return range;
}

token_type identifier_token::get_type() const
{
    return token_type::identifier;
}

std::string const& identifier_token::get_text() const
{
    return text;
}
