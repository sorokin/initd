#include "binary_stream.h"

#include <cstring>
#include <sstream>
#include <stdexcept>

binary_output_stream::binary_output_stream()
{}

void binary_output_stream::clear()
{
    buffer.clear();
}

void binary_output_stream::write(void const* data, size_t size)
{
    for (char const* i   = static_cast<char const*>(data),
                   * end = i + size; i != end; ++i)
        buffer.push_back(*i);
}

void const* binary_output_stream::data() const
{
    return buffer.data();
}

size_t binary_output_stream::size() const
{
    return buffer.size();
}

binary_input_stream::binary_input_stream(void const* data, size_t size)
    : begin(static_cast<char const*>(data))
    , pos(begin)
    , end(begin + size)
{}

bool binary_input_stream::eof() const
{
    return pos == end;
}

size_t binary_input_stream::left() const
{
    return end - pos;
}

void binary_input_stream::advance(size_t size)
{
    size_t avail = left();
    if (avail < size)
    {
        std::stringstream ss;
        ss << "advance past the end of stream, bytes left: " << avail << ", size: " << size;
        throw std::runtime_error(ss.str());
    }

    pos += size;
}

void binary_input_stream::read(void* data, size_t size)
{
    size_t avail = left();
    if (avail < size)
    {
        std::stringstream ss;
        ss << "read past the end of stream, bytes left: " << avail << ", size: " << size;
        throw std::runtime_error(ss.str());
    }

    ::memcpy(data, pos, size);
    pos += size;
}

void binary_input_stream::expect_eof()
{
    if (!eof())
    {
        std::stringstream ss;
        ss << "expected eof, but " << left() << " bytes left";
        throw std::runtime_error(ss.str());
    }
}
