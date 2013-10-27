#ifndef STRING_LITERAL_NODE_H
#define STRING_LITERAL_NODE_H

#include "token.h"
#include "text_range.h"
#include <string>

struct string_literal_token : token
{
    string_literal_token(text_range const&, std::string value);

    text_range get_range() const;
    token_type get_type() const;

    std::string const& get_value() const;

private:
    text_range range;
    std::string value;
};

typedef std::unique_ptr<string_literal_token> string_literal_token_sp;

#endif
