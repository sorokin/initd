#ifndef MOUNT_OPTIONS_H
#define MOUNT_OPTIONS_H

enum class mount_options
{
    none         = 0,

    dirsync      = 0x001,
    mandlock     = 0x002,
    noatime      = 0x004,
    nodev        = 0x008,
    nodiratime   = 0x010,
    noexec       = 0x020,
    nosuid       = 0x040,
    rdonly       = 0x080,
    relatime     = 0x100,
    silent       = 0x200,
    strictatime  = 0x400,
};

enum class umount_options
{
    none         = 0,

    lazy         = 0x001,
};

mount_options operator&(mount_options, mount_options);
mount_options operator|(mount_options, mount_options);
umount_options operator&(umount_options, umount_options);
umount_options operator|(umount_options, umount_options);

#endif // MOUNT_OPTIONS_H
