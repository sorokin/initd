#ifndef CTL_PROTOCOL_H
#define CTL_PROTOCOL_H

#include "binary_stream.h"
#include <cstdint>
#include <string>

struct reboot_msg
{
    static uint32_t const msg_id;
};

struct power_off_msg
{
    static uint32_t const msg_id;
};

struct set_runlevel_msg
{
    static uint32_t const msg_id;

    set_runlevel_msg();
    explicit set_runlevel_msg(std::string runlevel_name);

    std::string runlevel_name;
};

size_t measure(set_runlevel_msg const& value);
void write(binary_output_stream& s, set_runlevel_msg const& value);
void read(binary_input_stream& s, set_runlevel_msg& value);

template <typename T>
void serialize(binary_output_stream& s, T const& value)
{
    uint32_t size = measure(value);
    write(s, size);
    write(s, T::msg_id);
    write(s, value);
}

#endif
