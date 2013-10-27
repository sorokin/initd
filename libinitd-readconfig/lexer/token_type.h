#ifndef NODE_TYPE_H
#define NODE_TYPE_H

enum class token_type
{
    unknown,
    eof,
    error,
    whitespace,
    identifier,
    integer_literal,
    string_literal,
    lbrace,
    rbrace,
    equals,
    semicolon,
    comma,
    task_keyword,
    true_keyword,
    false_keyword,
    depends_keyword,
};

#endif
