#ifndef IDENTIFIER_TOKEN_H
#define IDENTIFIER_TOKEN_H

#include "token.h"
#include "text_range.h"

#include <string>

struct identifier_token : token
{
    identifier_token(text_range const&, std::string text);

    text_range get_range() const;
    token_type get_type() const;
    std::string const& get_text() const;

private:
    text_range range;
    std::string text;
};

typedef std::unique_ptr<identifier_token> identifier_token_sp;

#endif
