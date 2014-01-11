#ifndef BINARY_STREAM_H
#define BINARY_STREAM_H

#include <type_traits>
#include <vector>
#include <string>
#include <stdexcept>

#include <boost/utility/enable_if.hpp>

struct binary_output_stream
{
    binary_output_stream();

    void clear();
    void write(void const* data, size_t size);

    void const* data() const;
    size_t size() const;

private:
    std::vector<char> buffer;
};

template <typename T>
typename boost::enable_if_c<std::is_trivial<T>::value, size_t>::type measure(T const& value)
{
    return sizeof value;
}

template <typename T>
typename boost::enable_if_c<std::is_trivial<T>::value, void>::type write(binary_output_stream& s, T const& value)
{
    s.write(&value, sizeof value);
}

template <typename Char, typename Traits, typename Allocator>
size_t measure(std::basic_string<Char, Traits, Allocator> const& value)
{
    return sizeof(uint32_t) + sizeof(Char) * value.size();
}

template <typename Char, typename Traits, typename Allocator>
void write(binary_output_stream& s, std::basic_string<Char, Traits, Allocator> const& value)
{
    uint32_t size32 = (uint32_t)value.size();
    if (size32 != value.size())
        throw std::runtime_error("too large string to serialize");

    write(s, size32);
    s.write(value.data(), sizeof(Char) * value.size());
}

struct binary_input_stream
{
    binary_input_stream(void const* data, size_t size);

    bool eof() const;
    size_t left() const;
    void advance(size_t size);
    void read(void* data, size_t size);
    void expect_eof();

private:
    char const* begin;
    char const* pos;
    char const* end;
};

template <typename T>
typename boost::enable_if_c<std::is_trivial<T>::value, void>::type read(binary_input_stream& s, T& value)
{
    s.read(&value, sizeof value);
}

template <typename Char, typename Traits, typename Allocator>
void read(binary_input_stream& s, std::basic_string<Char, Traits, Allocator>& value)
{
    uint32_t size32;
    read(s, size32);

    std::vector<Char> tmp;
    tmp.resize(size32);
    s.read(tmp.data(), sizeof(Char) * size32);

    value.assign(tmp.begin(), tmp.end());
}

#endif
