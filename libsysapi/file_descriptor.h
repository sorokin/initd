#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <string>
#include <vector>

#include <sys/stat.h>

namespace sysapi
{
    struct file_descriptor
    {
        file_descriptor();
        file_descriptor(int fd);
        ~file_descriptor();

        static file_descriptor open(std::string const& filename, int flags, mode_t mode = 0);
        static file_descriptor open_if_exists(std::string const& filename, int flags);

        file_descriptor(file_descriptor const&) = delete;
        file_descriptor(file_descriptor&&);

        file_descriptor& operator=(file_descriptor);

        void swap(file_descriptor& other);

        int getfd() const;

        size_t read_some(void* buf, size_t count);
        void read(void* buf, size_t count);

        size_t write_some(void const* buf, size_t count);
        void write(void const* buf, size_t count);

        struct stat get_stat();

    private:
        int fd_;
    };

    std::pair<file_descriptor, file_descriptor> create_pipe(int flags);

    std::vector<char> read_entire_file(std::string const& filename);
}

#endif // FILE_DESCRIPTOR_H
