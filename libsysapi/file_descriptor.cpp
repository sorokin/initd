#include "file_descriptor.h"

#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "errors.h"

using namespace sysapi;

file_descriptor::file_descriptor()
    : fd_(-1)
{}

file_descriptor::~file_descriptor()
{
    if (fd_ == -1)
        return;

    int r = close(fd_);
    if (r != 0)
    {
        // if close returns EINTR the file descriptor _will_ be closed, no need to try to close again

        int err = errno;

        std::stringstream ss;
        ss << "unable to close file descriptor \"" << fd_ << "\", error: " << sysapi::errno_to_text(err);

        std::cerr << ss.str() << std::endl;
    }
}

file_descriptor file_descriptor::open(std::string const &filename, int flags, mode_t mode)
{
    int fd = ::open(filename.c_str(), flags, mode);

    if (fd == -1)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to open file \"" << filename << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(fd);
}

file_descriptor file_descriptor::open_if_exists(std::string const& filename, int flags)
{
    int fd = ::open(filename.c_str(), flags);

    if (fd == -1)
    {
        int err = errno;

        if (err == ENOENT)
            return file_descriptor(-1);

        std::stringstream ss;
        ss << "unable to open file \"" << filename << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return file_descriptor(fd);
}

file_descriptor::file_descriptor(file_descriptor&& rhs)
    : fd_(rhs.fd_)
{
    rhs.fd_ = -1;
}

file_descriptor& file_descriptor::operator=(file_descriptor rhs)
{
    swap(rhs);
    return *this;
}

void file_descriptor::swap(file_descriptor& other)
{
    std::swap(fd_, other.fd_);
}

int file_descriptor::getfd() const
{
    return fd_;
}

size_t file_descriptor::read_some(void* buf, size_t count)
{
    ssize_t nread = ::read(fd_, buf, count);
    if (nread < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to read file descriptor \"" << fd_ << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return (size_t)nread;
}

void file_descriptor::read(void* buf, size_t count)
{
    size_t nread = read_some(buf, count);
    if (nread != count)
    {
        std::stringstream ss;
        ss << "unexpected end of file, requested \"" << count << "\" bytes, read: \"" << nread << "\" bytes";

        throw std::runtime_error(ss.str());
    }
}

size_t file_descriptor::write_some(void const* buf, size_t count)
{
    ssize_t nwritten = ::write(fd_, buf, count);
    if (nwritten < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to write file descriptor \"" << fd_ << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return (size_t)nwritten;
}

void file_descriptor::write(void const* buf, size_t count)
{
    size_t nwritten = write_some(buf, count);
    if (nwritten != count)
    {
        std::stringstream ss;
        ss << "incomplete write, requested \"" << count << "\" bytes, written: \"" << nwritten << "\" bytes";

        throw std::runtime_error(ss.str());
    }
}

struct stat file_descriptor::get_stat()
{
    struct stat buf;
    int res = fstat(fd_, &buf);

    if (res < 0)
    {
        int err = errno;

        std::stringstream ss;
        ss << "unable to read stats of file descriptor \"" << fd_ << "\", error: " << sysapi::errno_to_text(err);

        throw std::runtime_error(ss.str());
    }

    return buf;
}

file_descriptor::file_descriptor(int fd)
    : fd_(fd)
{}

std::vector<char> sysapi::read_entire_file(std::string const& filename)
{
    auto fd = sysapi::file_descriptor::open(filename, O_RDONLY | O_CLOEXEC);
    struct stat stat = fd.get_stat();

    std::vector<char> res((size_t)stat.st_size);
    fd.read(res.data(), res.size());

    return res;
}
