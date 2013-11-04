#include "task_handle.h"
#include "initd_state.h"

#include "make_unique.h"

#include <stdexcept>
#include <iostream>

#include "read_value_node/read_task_description.h"

#include "epoll.h"
#include "signalfd.h"
#include "function_queue.h"
#include "block_signals.h"

#include <signal.h>

void enqueue_work(sysapi::function_queue* fq, initd_state* state)
{
    if (state->has_pending_operations())
        fq->push([=]() {
            state->run_once();
            enqueue_work(fq, state);
        });
}

int main(int argc, char* argv[])
{
    try
    {
        std::string config_filename = argc >= 2 ? argv[1] : "default.initd.conf";

        task_descriptions descriptions = read_descriptions(config_filename, std::cerr);
        run_level rlevel = run_level::all(descriptions);

        sysapi::epoll epoll;

        bool done = false;

        sigset_t mask = sysapi::make_sigset({SIGINT, SIGTERM});
        sysapi::block_signals block_signals(mask);
        sysapi::signalfd sfd(epoll, mask, [&done](signalfd_siginfo const& info) {
            done = true;
        });

        sysapi::function_queue fq(epoll);

        initd_state state(std::move(descriptions));
        state.set_run_level(rlevel);

        enqueue_work(&fq, &state);

        while (!done)
            epoll.wait();

        std::cerr << "setting empty run_level..." << std::endl;
        state.set_run_level(run_level{});

        while (state.has_pending_operations())
            state.run_once();

        std::cerr << "all tasks were shut down, terminating..." << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
