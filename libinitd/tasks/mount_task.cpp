#include "mount_task.h"

#include "task_handle.h"
#include "make_unique.h"
#include "errors.h"
#include "loop_device.h"
#include "file_descriptor.h"

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <sys/mount.h>
#include <fcntl.h>

namespace
{
    unsigned long mount_options_to_system_flags(mount_options moptions)
    {
        unsigned long result = 0;

        if ((moptions & mount_options::dirsync) != mount_options::none)
            result |= MS_DIRSYNC;
        if ((moptions & mount_options::mandlock) != mount_options::none)
            result |= MS_MANDLOCK;
        if ((moptions & mount_options::noatime) != mount_options::none)
            result |= MS_NOATIME;
        if ((moptions & mount_options::nodev) != mount_options::none)
            result |= MS_NODEV;
        if ((moptions & mount_options::nodiratime) != mount_options::none)
            result |= MS_NODIRATIME;
        if ((moptions & mount_options::noexec) != mount_options::none)
            result |= MS_NOEXEC;
        if ((moptions & mount_options::nosuid) != mount_options::none)
            result |= MS_NOSUID;
        if ((moptions & mount_options::rdonly) != mount_options::none)
            result |= MS_RDONLY;
        if ((moptions & mount_options::relatime) != mount_options::none)
            result |= MS_RELATIME;
        if ((moptions & mount_options::silent) != mount_options::none)
            result |= MS_SILENT;
        if ((moptions & mount_options::strictatime) != mount_options::none)
            result |= MS_STRICTATIME;

        return result;
    }

    int umount_options_to_system_flags(umount_options uoptions)
    {
        int result = 0;

        if ((uoptions & umount_options::lazy) != umount_options::none)
            result |= MNT_DETACH;

        return result;
    }

    void mount_filesystem(std::string const& device_name, std::string const& mount_point, std::string const& filesystem_type, mount_options moptions)
    {
        int r = mount(device_name.c_str(), mount_point.c_str(), filesystem_type.c_str(), mount_options_to_system_flags(moptions), nullptr);
        if (r != 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to mount filesystem \"" << device_name << "\" to \"" << mount_point << "\", error: " << sysapi::errno_to_text(err);

            switch (err)
            {
            case EACCES:
                ss << ", a component of path was not searchable or mounting a read-only file system without RDONLY flag or block device is located on a file system mounted with NODEV option";
                break;
            case EBUSY:
                ss << ", device is already mounted or mount point is busy";
                break;
            case EINVAL:
                ss << ", device has invalid super block";
                break;
            case EMFILE:
                ss << ", table of dummy devices is full";
                break;
            case ENODEV:
                ss << ", file system type is not configured in the kernel";
                break;
            case EPERM:
                ss << ", you must be a root or have the CAP_SYS_ADMIN capability to mount a file system";
                break;
            case ENOTBLK:
                ss << ", \"" << device_name << "\" is not a block device";
                break;
            }

            throw std::runtime_error(ss.str());
        }
    }

    void umount_filesystem(std::string const& mount_point, umount_options uoptions)
    {
        int r = umount2(mount_point.c_str(), umount_options_to_system_flags(uoptions));
        if (r != 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to umount filesystem \"" << mount_point << "\", error: " << sysapi::errno_to_text(err);

            switch (err)
            {
            case EBUSY:
                ss << ", device could not be unmounted because it is busy";
                break;
            case EINVAL:
                ss << ", path is not a mount point";
                break;
            case EPERM:
                ss << ", you must be a root or have the CAP_SYS_ADMIN capability to unmount a file system";
                break;
            }

            throw std::runtime_error(ss.str());
        }
    }

    struct mounting_guard
    {
        mounting_guard(std::string const& device_name,
                       std::string const& mount_point,
                       std::string const& filesystem_type,
                       mount_options moptions,
                       umount_options uoptions)
            : mount_point_(mount_point)
            , uoptions_(uoptions)
        {
            std::cerr << "mounting \"" << device_name << "\" to \"" << mount_point << "\"..." << std::endl;
            mount_filesystem(device_name, mount_point, filesystem_type, moptions);
        }

        ~mounting_guard()
        {
            try
            {
                std::cerr << "unmounting \"" << mount_point_ << "\"..." << std::endl;
                umount_filesystem(mount_point_, uoptions_);
            }
            catch (std::exception const& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        std::string mount_point() const
        {
            return mount_point_;
        }

    private:
        std::string mount_point_;
        umount_options uoptions_;
    };

    std::string loop_device_filename(size_t index)
    {
        std::stringstream ss;
        ss << "/dev/loop" << index;
        return ss.str();
    }

    struct loop_device_association_guard
    {
        loop_device_association_guard(std::string const& filename)
        {
            auto fd = sysapi::file_descriptor::open(filename, O_RDWR);

            for (size_t i = 0; i < 8; ++i)
            {
                std::string loop_device_fn = loop_device_filename(i);
                sysapi::loop_device loop_device_fd(loop_device_fn);
                if (loop_device_fd.try_setfd(fd))
                {
                    associated_loop_device_ = std::move(loop_device_fn);
                    std::cerr << "attach \"" << associated_loop_device_ << "\" to \"" << filename << "\"" << std::endl;
                    return;
                }
            }

            throw std::runtime_error("no free loop device found");
        }

        ~loop_device_association_guard()
        {
            try
            {
                std::cerr << "detach \"" << associated_loop_device_ << "\"" << std::endl;
                sysapi::loop_device loop_device_fd(associated_loop_device_);
                loop_device_fd.clearfd();
            }
            catch (std::exception const& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        std::string const& loop_device() const
        {
            return associated_loop_device_;
        }

    private:
        std::string associated_loop_device_;
    };

    struct mount_task_handle : task_handle
    {
        mount_task_handle(mount_task_data const& data)
            : guard(data.device_name, data.mount_point, data.filesystem_type, data.moptions, data.uoptions)
        {}

        std::string status_message() const
        {
            std::stringstream ss;
            ss << "mounted: \"" << guard.mount_point() << "\"";
            return ss.str();
        }

    private:
        mounting_guard guard;
    };

    struct loop_mount_task_handle : task_handle
    {
        loop_mount_task_handle(mount_task_data const& data)
            : loop_guard(data.device_name)
            , mount_guard(loop_guard.loop_device(), data.mount_point, data.filesystem_type, data.moptions, data.uoptions)
        {}

        std::string status_message() const
        {
            std::stringstream ss;
            ss << "mounted: \"" << mount_guard.mount_point() << "\" over \"" << loop_guard.loop_device() << "\"";
            return ss.str();
        }

    private:
        loop_device_association_guard loop_guard;
        mounting_guard mount_guard;
    };
}

task_handle_ptr create_mount_task(mount_task_data const& data)
{
    return make_unique<mount_task_handle>(data);
}

task_handle_ptr create_loop_mount_task(mount_task_data const& data)
{
    return make_unique<loop_mount_task_handle>(data);
}
