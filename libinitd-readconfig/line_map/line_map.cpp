#include "line_map.h"

#include <algorithm>
#include <cassert>

line_map::line_map(text_range const& whole_file)
    : file_end(whole_file.get_end())
{
    line_breaks.push_back(whole_file.get_start());
    for (char const* i = whole_file.get_start(); i != whole_file.get_end(); ++i)
        if (*i == '\n')
            line_breaks.push_back(i + 1);
}

line_map::line_column line_map::get_position(char const* pos) const
{
    auto i = std::upper_bound(line_breaks.begin(), line_breaks.end(), pos);

    assert(i != line_breaks.begin());
    --i;

    return line_column{unsigned(i - line_breaks.begin()), unsigned(pos - *i)};
}

text_range line_map::get_line_range(unsigned line) const
{
    if ((line + 1) < line_breaks.size())
        return text_range{line_breaks[line], line_breaks[line + 1]};
    else
        return text_range{line_breaks[line], file_end};
}

text_range line_map::get_line_range_wo_newline(unsigned line) const
{
    text_range r = get_line_range(line);
    if (*(r.get_end() - 1) == '\n')
        r = text_range(r.get_start(), r.get_end() - 1);
    return r;
}
