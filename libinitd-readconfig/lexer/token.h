#ifndef NODE_H
#define NODE_H

#include "token_type.h"
#include "token_fwd.h"

struct text_range;

struct token
{
    virtual token_type get_type() const = 0;
    virtual text_range get_range() const = 0;
    virtual ~token();
};

#endif
