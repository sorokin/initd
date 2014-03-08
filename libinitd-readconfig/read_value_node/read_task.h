#ifndef READ_TASK_DATA_H
#define READ_TASK_DATA_H

#include <boost/optional.hpp>

#include "task_data.h"
#include "parser/task_node_fwd.h"

struct error_tag_sink;

boost::optional<task_data> read_task(task_node const& node, error_tag_sink& esink);

#endif
