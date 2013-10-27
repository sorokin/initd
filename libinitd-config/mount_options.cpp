#include "mount_options.h"

#include <type_traits>

mount_options operator&(mount_options a, mount_options b)
{
    typedef std::underlying_type<mount_options>::type utype;

    return static_cast<mount_options>(static_cast<utype>(a) & static_cast<utype>(b));
}

mount_options operator|(mount_options a, mount_options b)
{
    typedef std::underlying_type<mount_options>::type utype;

    return static_cast<mount_options>(static_cast<utype>(a) | static_cast<utype>(b));
}

umount_options operator&(umount_options a, umount_options b)
{
    typedef std::underlying_type<umount_options>::type utype;

    return static_cast<umount_options>(static_cast<utype>(a) & static_cast<utype>(b));
}

umount_options operator|(umount_options a, umount_options b)
{
    typedef std::underlying_type<umount_options>::type utype;

    return static_cast<umount_options>(static_cast<utype>(a) | static_cast<utype>(b));
}
