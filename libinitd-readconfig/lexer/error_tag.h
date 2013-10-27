#ifndef ERROR_TAG_H
#define ERROR_TAG_H

#include <string>
#include <vector>
#include "text_range.h"

struct error_tag
{
    error_tag(text_range const&, std::string const& message);

    text_range get_range() const;
    std::string const& get_message() const;

private:
    text_range range;
    std::string message;
};

struct error_tag_sink
{
    void push(error_tag);

    std::vector<error_tag> const& get_errors();

private:
    std::vector<error_tag> data;
};

#endif
