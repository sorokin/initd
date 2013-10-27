#include "print_error.h"

#include "line_map.h"
#include "lexer/error_tag.h"

void print_error(std::ostream& os, std::string const& filename, line_map const& lmap, error_tag const& e)
{
    if (e.get_range().get_start() == e.get_range().get_end())
    {
        line_map::line_column pos = lmap.get_position(e.get_range().get_start());

        os << filename
           << ":" << pos.line << ":" << pos.column
           << ": " << e.get_message()<< std::endl;

        text_range line_range = lmap.get_line_range_wo_newline(pos.line);
        os << std::string(line_range.get_start(), line_range.get_end()) << std::endl;
        os << std::string(pos.column, ' ') << "^" << std::endl;
    }
    else
    {
        line_map::line_column start = lmap.get_position(e.get_range().get_start());
        line_map::line_column end   = lmap.get_position(e.get_range().get_end());

        if (start.line == end.line)
        {
            os << filename
               << ":" << start.line << ":" << start.column
               << "~" << end.column
               << ": " << e.get_message()<< std::endl;

            text_range line_range = lmap.get_line_range_wo_newline(start.line);
            os << std::string(line_range.get_start(), line_range.get_end()) << std::endl;
            os << std::string(start.column, ' ') << std::string(end.column - start.column, '~') << std::endl;
        }
        else
        {
            os << filename
               << ":" << start.line << ":" << start.column
               << "~" << end.line   << ":" << end.column
               << ": " << e.get_message()<< std::endl;

            text_range line_range = lmap.get_line_range_wo_newline(start.line);
            os << std::string(line_range.get_start(), line_range.get_end()) << std::endl;
            size_t line_size = line_range.get_end() - line_range.get_start();
            os << std::string(start.column, ' ') << std::string(line_size - start.column, '~') << std::endl;
        }
    }
}
