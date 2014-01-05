#ifndef BINARY_STREAM_H
#define BINARY_STREAM_H

#include <type_traits>
#include <vector>

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
typename boost::enable_if_c<std::is_trivial<T>::value, void>::type write(binary_output_stream& s, T const& value)
{
    s.write(&value, sizeof value);
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


#endif
