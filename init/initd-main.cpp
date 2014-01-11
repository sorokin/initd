#include "initd_state.h"
#include "state_context.h"

#include "make_unique.h"

#include <stdexcept>
#include <iostream>

#include "read_value_node/read_task_description.h"

#include "epoll.h"
#include "signalfd.h"
#include "block_signals.h"
#include "sigchild_handler.h"
#include "reboot.h"

#include <signal.h>
#include <unistd.h>
#include <unistd.h>

namespace
{
    struct context : state_context
    {
        enum class target
        {
            running,
            reboot,
            power_off,
        };

        context(task_descriptions descriptions);

        bool is_active() const;

        void reboot();
        void power_off();
        void set_runlevel(std::string const&);

        target get_target() const;
        sysapi::epoll& get_epoll();
        initd_state& get_state();

    private:
        target tg;
        sysapi::epoll epoll;
        sysapi::block_signals block_signals;
        sysapi::signalfd sfd;
        sysapi::install_sigchild_handler install_sigchild_handler;
        initd_state state;
    };

    const sigset_t sig_mask = sysapi::make_sigset({SIGINT, SIGTERM});

    context::context(task_descriptions descriptions)
        : tg(target::running)
        , block_signals(sig_mask)
        , sfd(epoll, sig_mask, [this](signalfd_siginfo const& info) {
            reboot();
        })
        , install_sigchild_handler(epoll)
        , state(*this, epoll, std::move(descriptions))
    {
        state.set_run_level("default");
    }

    bool context::is_active() const
    {
        return tg == target::running || state.has_pending_operations();
    }

    void context::reboot()
    {
        tg = target::reboot;
        state.set_empty_run_level();
    }

    void context::power_off()
    {
        tg = target::power_off;
        state.set_empty_run_level();
    }

    void context::set_runlevel(std::string const& runlevel_name)
    {
        std::cerr << "setting runlevel: " << runlevel_name << std::endl;
        tg = target::running;
        state.set_run_level(runlevel_name);
    }

    context::target context::get_target() const
    {
        return tg;
    }

    sysapi::epoll& context::get_epoll()
    {
        return epoll;
    }
}

int main(int argc, char* argv[])
{
    context::target exit_state;

    try
    {
        std::string config_filename = argc >= 2 ? argv[1] : "default.initd.conf";

        context ctx(read_descriptions(config_filename, std::cerr));

        while (ctx.is_active())
            ctx.get_epoll().wait();

        exit_state = ctx.get_target();
        std::cerr << "all tasks were shut down, terminating..." << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        exit_state = context::target::reboot;
    }

    if (getpid() == 1)
    {
        try
        {
            switch (exit_state)
            {
            case context::target::running:
                assert(false);
                break;
            case context::target::reboot:
                std::cout << "syncing drives..." << std::endl;
                sync();
                std::cout << "reboot..." << std::endl;
                sysapi::reboot();
                break;
            case context::target::power_off:
                std::cout << "syncing drives..." << std::endl;
                sync();
                std::cout << "power off..." << std::endl;
                sysapi::power_off();
                break;
            default:
                assert(false);
                break;
            }
        }
        catch (std::exception const& e)
        {
            std::cerr << "error: " << e.what() << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
