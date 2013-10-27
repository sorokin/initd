#include "task_handle.h"
#include "initd_state.h"

#include "make_unique.h"

#include <stdexcept>
#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>

#include "read_value_node/read_task_description.h"

struct initd_program
{
    initd_program(boost::asio::io_service& ios, task_descriptions&& descriptions, run_level const& rlevel);

    void enqueue_work();

private:
    boost::asio::io_service& ios;
    boost::optional<boost::asio::io_service::work> work;
    boost::asio::signal_set signal_set;
    initd_state state;
    bool want_quit;
};

initd_program::initd_program(boost::asio::io_service &ios, task_descriptions&& descriptions, run_level const& rlevel)
    : ios(ios)
    , work(boost::in_place(boost::ref(ios)))
    , signal_set(ios, SIGINT, SIGTERM)
    , state(std::move(descriptions))
    , want_quit(false)
{
    state.set_run_level(rlevel);

    signal_set.async_wait(
        [&](boost::system::error_code const&, int signal_number)
        {
            std::cerr << "setting empty run_level..." << std::endl;
            want_quit = true;
            state.set_run_level(run_level{});
            enqueue_work();
        });

    enqueue_work();
}

void initd_program::enqueue_work()
{
    if (state.has_pending_operations())
        ios.dispatch(
            [&]
            {
                state.run_once();
                enqueue_work();
            });
    else
    {
        if (want_quit)
        {
            std::cerr << "all tasks were shut down, terminating..." << std::endl;
            work = boost::none;
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::string config_filename = argc >= 2 ? argv[1] : "default.initd.conf";

        boost::asio::io_service ios;

        task_descriptions descriptions = read_descriptions(config_filename, std::cerr);
    /*    descriptions.all_tasks.push_back(make_unique<task_description>(&create_hostname_task, hostname_task_data{"test", false}));
        mount_task_data mntb{"/dev/disk/by-uuid/46a9bb65-c009-45d4-aef3-13b94c1ba447", "/mnt/b", "xfs",
                             mount_options::noexec | mount_options::noatime,
                             umount_options::none};
        descriptions.all_tasks.push_back(make_unique<task_description>(&create_mount_task, mntb));
        mount_task_data mnttmp{"/home/ivan/tmp/fs", "/mnt/tmp", "ext4",
                             mount_options::noexec | mount_options::noatime,
                             umount_options::none};
        descriptions.all_tasks.push_back(make_unique<task_description>(&create_loop_mount_task, mnttmp));*/
        //start_stop_task_data echotest{{"/bin/ls"}, {"/bin/echo", "echo test stop"}};
        //descriptions.all_tasks.push_back(make_unique<task_description>("test", echotest));

        run_level rlevel = run_level::all(descriptions);

        initd_program initd(ios, std::move(descriptions), rlevel);

        ios.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
