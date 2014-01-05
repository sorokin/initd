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
        enum class state
        {
            running,
            reboot,
            power_off,
        };

        context();

        void reboot();
        void power_off();

        state get_state() const;

    private:
        state st;
    };

    context::context()
        : st(state::running)
    {}

    void context::reboot()
    {
        st = state::reboot;
    }

    void context::power_off()
    {
        st = state::power_off;
    }

    context::state context::get_state() const
    {
        return st;
    }
}

int main(int argc, char* argv[])
{
    context::state exit_state;

    try
    {
        std::string config_filename = argc >= 2 ? argv[1] : "default.initd.conf";

        task_descriptions descriptions = read_descriptions(config_filename, std::cerr);

        sysapi::epoll epoll;

        context ctx;

        sigset_t mask = sysapi::make_sigset({SIGINT, SIGTERM});
        sysapi::block_signals block_signals(mask);
        sysapi::signalfd sfd(epoll, mask, [&ctx](signalfd_siginfo const& info) {
            ctx.reboot();
        });

        sysapi::install_sigchild_handler install_sigchild_handler(epoll);

        initd_state state(ctx, epoll, std::move(descriptions));
        state.set_run_level("default");

        while (ctx.get_state() == context::state::running)
            epoll.wait();

        std::cerr << "setting empty run_level..." << std::endl;
        state.set_empty_run_level();

        while (state.has_pending_operations())
            epoll.wait();

        exit_state = ctx.get_state();
        std::cerr << "all tasks were shut down, terminating..." << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        exit_state = context::state::reboot;
    }

    if (getpid() == 1)
    {
        try
        {
            switch (exit_state)
            {
            case context::state::running:
                assert(false);
                break;
            case context::state::reboot:
                std::cout << "syncing drives..." << std::endl;
                sync();
                std::cout << "reboot..." << std::endl;
                sysapi::reboot();
                break;
            case context::state::power_off:
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
