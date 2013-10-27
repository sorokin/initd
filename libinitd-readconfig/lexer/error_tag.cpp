#include "error_tag.h"

error_tag::error_tag(text_range const& range, const std::string& message)
    : range(range)
    , message(message)
{}

text_range error_tag::get_range() const
{
    return range;
}

const std::string &error_tag::get_message() const
{
    return message;
}

void error_tag_sink::push(error_tag et)
{
    data.push_back(std::move(et));
}

std::vector<error_tag> const& error_tag_sink::get_errors()
{
    return data;
}
