#ifndef CTL_PROTOCOL_H
#define CTL_PROTOCOL_H

#include "binary_stream.h"
#include <cstdint>

struct reboot_msg
{
    static uint32_t const msg_id;
};

struct power_off_msg
{
    static uint32_t const msg_id;
};

template <typename T>
void serialize(binary_output_stream& s, T const& value)
{
    uint32_t size = sizeof value;
    write(s, size);
    write(s, T::msg_id);
    write(s, value);
}

#endif
