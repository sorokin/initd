#ifndef PARSER_H
#define PARSER_H

#include "top_level_node_fwd.h"

struct lexer;
struct error_tag_sink;

top_level_node_sp parse_file(lexer&, error_tag_sink& esink);

#endif
