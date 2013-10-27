#ifndef TASK_DATA_H
#define TASK_DATA_H

#include <boost/variant.hpp>

#include "mount_task_data.h"
#include "hostname_task_data.h"
#include "start_stop_task_data.h"
#include "null_task_data.h"

typedef boost::variant<mount_task_data, hostname_task_data, start_stop_task_data, null_task_data> task_data;

#endif
