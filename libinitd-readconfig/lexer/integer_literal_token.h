#ifndef INTEGER_LITERAL_NODE_H
#define INTEGER_LITERAL_NODE_H

#include "token.h"
#include "text_range.h"

struct integer_literal_token : token
{
    integer_literal_token(text_range const&, int value);

    text_range get_range() const;
    token_type get_type() const;

    int get_value() const;

private:
    text_range range;
    int value;
};

typedef std::unique_ptr<integer_literal_token> integer_literal_token_sp;

#endif
