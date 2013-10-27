#ifndef MOUNT_TASK_DATA_H
#define MOUNT_TASK_DATA_H

#include <string>
#include "mount_options.h"

struct mount_task_data
{
    std::string device_name;
    std::string mount_point;
    std::string filesystem_type;
    mount_options moptions;
    umount_options uoptions;
};

#endif // MOUNT_TASK_DATA_H
