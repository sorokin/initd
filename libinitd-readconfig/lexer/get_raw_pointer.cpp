#include "get_raw_pointer.h"

token* get_raw_pointer::operator()(token_sp const& n) const
{
    return n.get();
}
