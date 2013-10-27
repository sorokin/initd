#ifndef SIMPLE_TOKEN_H
#define SIMPLE_TOKEN_H

#include "token.h"
#include "text_range.h"

struct simple_token : token
{
    simple_token(text_range const&, token_type);

    text_range get_range() const;
    token_type get_type() const;

private:
    text_range range;
    token_type type;
};

#endif
