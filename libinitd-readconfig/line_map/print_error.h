#ifndef PRINT_ERROR_H
#define PRINT_ERROR_H

#include <istream>
#include <string>

struct line_map;
struct error_tag;

void print_error(std::ostream& os, std::string const& filename, line_map const& lmap, error_tag const& e);

#endif
