#include "ctl_protocol.h"

uint32_t const reboot_msg::msg_id       = 1;
uint32_t const power_off_msg::msg_id    = 2;
uint32_t const set_runlevel_msg::msg_id = 3;

set_runlevel_msg::set_runlevel_msg()
{}

set_runlevel_msg::set_runlevel_msg(std::string runlevel_name)
    : runlevel_name(runlevel_name)
{}

size_t measure(set_runlevel_msg const& value)
{
    return measure(value.runlevel_name);
}

void write(binary_output_stream& s, set_runlevel_msg const& value)
{
    write(s, value.runlevel_name);
}

void read(binary_input_stream& s, set_runlevel_msg& value)
{
    read(s, value.runlevel_name);
}
