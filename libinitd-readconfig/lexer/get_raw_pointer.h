#ifndef GET_RAW_POINTER_H
#define GET_RAW_POINTER_H

#include "token_fwd.h"

struct get_raw_pointer
{
    typedef token* result_type;

    token* operator()(token_sp const& n) const;
};

#endif
