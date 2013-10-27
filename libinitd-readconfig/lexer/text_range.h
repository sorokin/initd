#ifndef TEXT_RANGE_H
#define TEXT_RANGE_H

#include <cstdlib>

struct text_range
{
    text_range(char const* start, char const* end);

    static text_range make_empty(char const*);

    template <size_t N>
    static text_range make_by_literal(char const (&lit)[N])
    {
        static_assert(N != 0, "literal can not have size 0");
        return text_range(lit, lit + N - 1);
    }

    char const* get_start() const;
    char const* get_end() const;

private:
    char const* start;
    char const* end;
};

#endif
