#include <sstream>
#include <stdexcept>
#include <boost/optional.hpp>

#include "network/socket_address.h"
#include "network/socket_common.h"

#include "file_descriptor.h"

#include "binary_stream.h"
#include "ctl_protocol.h"

int main(int argc, char const* argv[])
{
    if (argc < 2)
    {
        std::cerr << argv[0] << " reboot\n";
        std::cerr << "    reboot computer\n";
        std::cerr << "\n";
        std::cerr << argv[0] << " poweroff\n";
        std::cerr << "    turn computer off\n";
        std::cerr << "\n";
        std::cerr << argv[0] << " runlevel <runlevel name>\n";
        std::cerr << "    set current runlevel\n";
        std::cerr << "\n";
        return EXIT_FAILURE;
    }

    try
    {
        sysapi::socket_address remote_address = sysapi::socket_address::unix_domain("/tmp/initd-control");

        sysapi::file_descriptor fd = sysapi::create_socket(remote_address.get_address_family(), SOCK_STREAM | SOCK_CLOEXEC, 0);
        sysapi::connect_socket(fd, remote_address);

        binary_output_stream stream;

        std::string action = argv[1];
        if (action == "reboot")
            serialize(stream, reboot_msg());
        else if (action == "poweroff")
            serialize(stream, power_off_msg());
        else if (action == "runlevel")
        {
            if (argc < 3)
                throw std::runtime_error("expected runlevel name");

            serialize(stream, set_runlevel_msg(argv[2]));
        }
        else
        {
            std::stringstream ss;
            ss << "unknown action: \"" << action << "\"";
            throw std::runtime_error(ss.str());
        }

        fd.write(stream.data(), stream.size());
    }
    catch (std::exception const& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
