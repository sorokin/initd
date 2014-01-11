#include "control_task.h"

#include "make_unique.h"
#include "task_handle.h"
#include "task_context.h"
#include "state_context.h"

#include "errors.h"
#include "network/stream_server_socket.h"
#include "network/stream_socket.h"

#include "binary_stream.h"
#include "ctl_protocol.h"

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <map>

#include <unistd.h>

namespace
{
    struct connection
    {
        typedef std::function<void ()> on_disconnect_t;

        connection(task_context& ctx, sysapi::file_descriptor fd, on_disconnect_t on_disconnect)
            : ctx(ctx)
            , ss(ctx.get_epoll(), std::move(fd))
            , rp(ss, [this]() {
                try
                {
                    if (rp.size() < 2 * sizeof(uint32_t))
                        return;

                    binary_input_stream s(rp.data(), rp.size());

                    uint32_t size;
                    read(s, size);

                    uint32_t const limit = 1024 * 1024;
                    if (size > limit)
                    {
                        std::stringstream ss;
                        ss << "control message is too long, size: " << size << ", limit: " << limit << std::endl;
                        throw std::runtime_error(ss.str());
                    }

                    uint32_t msg_id;
                    read(s, msg_id);

                    if (s.left() < size)
                        return;

                    if (msg_id == reboot_msg::msg_id)
                    {
                        reboot_msg msg;
                        read(s, msg);
                        s.expect_eof();
                        this->ctx.get_state_context().reboot();
                    }
                    else if (msg_id == power_off_msg::msg_id)
                    {
                        power_off_msg msg;
                        read(s, msg);
                        s.expect_eof();
                        this->ctx.get_state_context().power_off();
                    }
                    else if (msg_id == set_runlevel_msg::msg_id)
                    {
                        set_runlevel_msg msg;
                        read(s, msg);
                        s.expect_eof();
                        this->ctx.get_state_context().set_runlevel(msg.runlevel_name);
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "unknown message id: " << msg_id;
                        throw std::runtime_error(ss.str());
                    }

                    rp.clear_buffer();
                }
                catch (std::exception const& e)
                {
                    std::cerr << "error at handling control message: " << e.what() << std::endl;
                    this->on_disconnect();
                }
            })
            , on_disconnect(on_disconnect)
        {
            ss.set_on_disconnect(on_disconnect);
            std::cerr << "connection created" << std::endl;
        }

        ~connection()
        {
            ss.set_on_disconnect(on_disconnect_t());
            std::cerr << "connection destroyed" << std::endl;
        }

    private:
        task_context& ctx;
        sysapi::stream_socket ss;
        sysapi::read_process rp;
        on_disconnect_t on_disconnect;
    };

    struct control_task_handle : task_handle
    {
        control_task_handle(task_context& ctx, control_task_data const& data)
            : ctx(ctx)
            , data(data)
            , server_socket(ctx.get_epoll(), sysapi::socket_address::unix_domain(data.uds_filename), [this](sysapi::file_descriptor fd) {
                uint64_t id = next_id++;
                //std::cout << "connected " << id << std::endl;
                connections.insert(std::make_pair(id,
                                                  make_unique<connection>(this->ctx, std::move(fd), [this, id]() {
                    auto i = connections.find(id);
                    assert(i != connections.end());
                    connections.erase(i);
                    //std::cout << "disconnected " << id << std::endl;
                })));
            })
            , next_id()
        {}

        ~control_task_handle()
        {
            int r = ::unlink(data.uds_filename.c_str());
            if (r < 0)
            {
                int err = errno;

                std::stringstream ss;
                ss << "unable to unlink \"" << data.uds_filename << "\", error: " << sysapi::errno_to_text(err);

                std::cerr << ss.str() << std::endl;
            }
        }

        std::string status_message() const
        {
            std::stringstream ss;
            ss << "filename: " << data.uds_filename;
            return ss.str();
        }

    private:
        task_context& ctx;
        control_task_data data;
        sysapi::stream_server_socket server_socket;
        std::map<uint64_t, std::unique_ptr<connection> > connections;
        uint64_t next_id;
    };
}

task_handle_ptr create_task(task_context& ctx, control_task_data const& data)
{
    return make_unique<control_task_handle>(ctx, data);
}
