#ifndef READ_TASK_DESCRIPTION_H
#define READ_TASK_DESCRIPTION_H

#include <map>
#include <string>
#include "task_description_fwd.h"

struct top_level_node;
struct error_tag_sink;
struct task_descriptions;

task_descriptions read_descriptions(std::string const& config_filename, std::ostream& error_stream);

#endif
