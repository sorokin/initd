#ifndef LEXER_H
#define LEXER_H

#include "text_range.h"
#include "token_fwd.h"

struct error_tag_sink;

bool is_ascii_whitespace(char c);

struct lexer
{
    lexer(text_range const& range, error_tag_sink&);

    text_range get_range() const;

    bool eof_token() const;
    token& peek_token();
    token_sp read_token();
    void advance_token();

private:
    void skip_whitespace();
    token_sp read_next_token();

    bool eof_char() const;
    char peek_char();
    void advance_char(size_t n = 1);

    bool is_single_line_comment_start() const;
    bool is_single_line_comment_end() const;
    bool is_multi_line_comment_start() const;
    bool is_multi_line_comment_end() const;
    bool is_raw_string_literal_start() const;
    bool is_raw_string_literal_end(std::string const&) const;

private:
    char const* start;
    char const* end;
    char const* pos;

    token_sp current_token;
    error_tag_sink* error_sink;
};
#endif
