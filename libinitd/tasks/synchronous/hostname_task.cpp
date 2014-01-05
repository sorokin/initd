#include "hostname_task.h"
#include "task_handle.h"
#include "make_unique.h"
#include "errors.h"

#include <unistd.h>
#include <string.h>
#include <sstream>
#include <stdexcept>

#include <boost/optional.hpp>

namespace
{
    void set_hostname(std::string const& hname)
    {
        int r = sethostname(hname.c_str(), hname.size());
        if (r != 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to set hostname, error: " << sysapi::errno_to_text(err);

            switch (err)
            {
            case EPERM:
                ss << ", you must be a root or have the CAP_SYS_ADMIN capability to set a hostname";
                break;
            case EINVAL:
                ss << ", hostname is too long (" << hname.size() << ")";
                break;
            }

            throw std::runtime_error(ss.str());
        }
    }

    std::string get_hostname()
    {
        size_t const buf_size = 2048;

        char buf[buf_size];
        int r = gethostname(buf, buf_size);
        if (r != 0)
        {
            int err = errno;

            std::stringstream ss;
            ss << "unable to set hostname, error: " << sysapi::errno_to_text(err);

            throw std::runtime_error(ss.str());
        }

        return buf;
    }

    struct hostname_task_handle : task_handle
    {
        hostname_task_handle(hostname_task_data const& data)
        {
            std::cerr << "setting hostname to \"" << data.hostname << "\"..." << std::endl;
            if (data.no_restore)
                set_hostname(data.hostname);
            else
            {
                std::string t = get_hostname();
                set_hostname(data.hostname);
                old_hname = t;
            }
        }

        ~hostname_task_handle()
        {
            try
            {
                if (old_hname)
                {
                    std::cerr << "reverting hostname to \"" << *old_hname << "\"" << std::endl;
                    set_hostname(*old_hname);
                }
            }
            catch (std::exception const& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        std::string status_message() const
        {
            return "hostname is set";
        }

    private:
        boost::optional<std::string> old_hname;
    };
}

task_handle_ptr create_task(task_context& ctx, hostname_task_data const& data)
{
    return make_unique<hostname_task_handle>(data);
}
