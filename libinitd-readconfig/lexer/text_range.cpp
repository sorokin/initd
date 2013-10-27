#include "text_range.h"

text_range::text_range(char const* start, char const* end)
    : start(start)
    , end(end)
{}

text_range text_range::make_empty(char const* pos)
{
    return text_range(pos, pos);
}

char const* text_range::get_start() const
{
    return start;
}

char const* text_range::get_end() const
{
    return end;
}
