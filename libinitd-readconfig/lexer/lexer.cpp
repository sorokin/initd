#include "lexer.h"

#include <cassert>
#include "make_unique.h"
#include "simple_token.h"
#include "identifier_token.h"
#include "string_literal_token.h"
#include "integer_literal_token.h"
#include "token_type.h"
#include "error_tag.h"

namespace
{
    bool is_identifier_start(char c)
    {
        return (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || c == '_';
    }

    bool is_identifier_trail(char c)
    {
        return (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || (c >= '0' && c <= '9')
            || c == '_';
    }

    bool is_number(char c)
    {
        return c >= '0' && c <= '9';
    }

    int char_to_number(char c)
    {
        return c - '0';
    }

    token_sp make_identifier_token(text_range const& range, std::string text)
    {
        if (text == "task")
            return make_unique<simple_token>(range, token_type::task_keyword);
        if (text == "true")
            return make_unique<simple_token>(range, token_type::true_keyword);
        if (text == "false")
            return make_unique<simple_token>(range, token_type::false_keyword);
        if (text == "depends")
            return make_unique<simple_token>(range, token_type::depends_keyword);

        return make_unique<identifier_token>(range, std::move(text));
    }
}

bool is_ascii_whitespace(char c)
{
    return c >= '\0' && c <= ' ';
}

lexer::lexer(text_range const& range, error_tag_sink& error_sink)
    : start(range.get_start())
    , end(range.get_end())
    , pos(range.get_start())
    , error_sink(&error_sink)
{
    current_token = read_next_token();
}

text_range lexer::get_range() const
{
    return text_range{start, end};
}

bool lexer::eof_token() const
{
    return !current_token;
}

token& lexer::peek_token()
{
    return *current_token;
}

token_sp lexer::read_token()
{
    token_sp t = read_next_token();
    std::swap(t, current_token);
    return t;
}

void lexer::advance_token()
{
    current_token = read_next_token();
}

void lexer::skip_whitespace()
{
    for (;;)
    {
        if (eof_char())
            break;

        if (!is_ascii_whitespace(peek_char()))
            break;

        advance_char();
    }
}

token_sp lexer::read_next_token()
{
    for (;;)
    {
        skip_whitespace();

        if (eof_char())
            return token_sp();

        char const* lex_start = pos;

        if (is_ascii_whitespace(peek_char()))
        {
            advance_char();
            while (!eof_char() && is_ascii_whitespace(peek_char()))
                advance_char();

            return make_unique<simple_token>(text_range(lex_start, pos), token_type::whitespace);
        }
        else if (is_single_line_comment_start())
        {
            advance_char(2);
            for (;;)
            {
                if (eof_char() || is_single_line_comment_end())
                {
                    advance_char();
                    break;
                }
                else
                    advance_char();
            }
        }
        else if (is_multi_line_comment_start())
        {
            advance_char(2);
            for (;;)
            {
                if (eof_char())
                {
                    error_sink->push(error_tag(text_range::make_empty(pos), "unterminated comment"));
                    break;
                }
                else if (is_multi_line_comment_end())
                {
                    advance_char(2);
                    break;
                }
                else
                    advance_char();
            }
        }
        else if (is_identifier_start(peek_char()))
        {
            std::string s(1, peek_char());
            advance_char();
            while (!eof_char() && is_identifier_trail(peek_char()))
            {
                s += peek_char();
                advance_char();
            }

            return make_identifier_token(text_range(lex_start, pos), std::move(s));
        }
        else if (is_number(peek_char()))
        {
            int value = char_to_number(peek_char());
            advance_char();

            while (!eof_char() && is_number(peek_char()))
            {
                value = value * 10 + char_to_number(peek_char());
                advance_char();
            }

            return make_unique<integer_literal_token>(text_range(lex_start, pos), value);
        }
        else if (peek_char() == '\"')
        {
            advance_char();

            std::string value;
            std::vector<token_sp> errors;

            for (;;)
            {
                if (eof_char() || peek_char() == '\n')
                {
                    text_range r(lex_start, pos);
                    error_sink->push(error_tag(r, "unterminated string"));
                    return make_unique<string_literal_token>(r, std::move(value));
                }
                else if (peek_char() == '\"')
                {
                    advance_char();
                    return make_unique<string_literal_token>(text_range(lex_start, pos), std::move(value));
                }
                else if (peek_char() == '\\')
                {
                    const char* escape_start = pos;
                    advance_char();
                    if (!eof_char())
                    {
                        switch (peek_char())
                        {
                        case 'a':
                            value += '\a';
                            break;
                        case 'b':
                            value += '\b';
                            break;
                        case 'f':
                            value += '\f';
                            break;
                        case 'n':
                            value += '\n';
                            break;
                        case 'r':
                            value += '\r';
                            break;
                        case 't':
                            value += '\t';
                            break;
                        case 'v':
                            value += '\v';
                            break;
                        case '\\':
                            value += '\\';
                            break;
                        case '\'':
                            value += '\'';
                            break;
                        case '\"':
                            value += '\"';
                            break;
                        default:
                            error_sink->push(error_tag(text_range(escape_start, pos + 1), "invalid escape character"));
                            value += '\\';
                            value += peek_char();
                            break;
                        }
                        advance_char();
                    }
                }
                else
                {
                    value += peek_char();
                    advance_char();
                }
            }
        }
        else if (peek_char() == '{')
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::lbrace);
        }
        else if (peek_char() == '}')
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::rbrace);
        }
        else if (peek_char() == '=')
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::equals);
        }
        else if (peek_char() == ';')
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::semicolon);
        }
        else if (peek_char() == ',')
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::comma);
        }
        else
        {
            advance_char();
            return make_unique<simple_token>(text_range(lex_start, pos), token_type::unknown);
        }
    }
}

bool lexer::eof_char() const
{
    return pos == end;
}

char lexer::peek_char()
{
    assert(!eof_char());

    return *pos;
}

void lexer::advance_char(size_t n)
{
    assert((end - pos) >= (std::ptrdiff_t)n);

    pos += n;
}

bool lexer::is_single_line_comment_start() const
{
    if ((end - pos) < 2)
        return false;

    return *pos == '/' && *(pos + 1) == '/';
}

bool lexer::is_single_line_comment_end() const
{
    if (end == pos)
        return false;

    return *pos == '\n';
}

bool lexer::is_multi_line_comment_start() const
{
    if ((end - pos) < 2)
        return false;

    return *pos == '/' && *(pos + 1) == '*';
}

bool lexer::is_multi_line_comment_end() const
{
    if ((end - pos) < 2)
        return false;

    return *pos == '*' && *(pos + 1) == '/';
}
