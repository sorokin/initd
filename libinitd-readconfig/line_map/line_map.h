#ifndef LINE_MAP_H
#define LINE_MAP_H

#include "lexer/text_range.h"

#include <vector>

struct line_map
{
    struct line_column
    {
        unsigned line;
        unsigned column;
    };

    line_map(text_range const& whole_file);

    line_column get_position(char const*) const;
    text_range get_line_range(unsigned line) const;
    text_range get_line_range_wo_newline(unsigned line) const;

private:
    std::vector<char const*> line_breaks;
    char const* file_end;
};

#endif
