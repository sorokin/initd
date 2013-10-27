#include "parser.h"

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "lexer/identifier_token.h"
#include "lexer/integer_literal_token.h"
#include "lexer/error_tag.h"
#include "make_unique.h"

#include "value_node.h"
#include "integer_node.h"
#include "string_node.h"
#include "boolean_node.h"
#include "pseudo_identifier_value_node.h"
#include "struct_node.h"
#include "property_node.h"
#include "task_node.h"
#include "dependency_node.h"
#include "top_level_node.h"

#include <cassert>
#include <boost/noncopyable.hpp>

namespace
{
    template <token_type tt>
    struct token_traits;

    template <>
    struct token_traits<token_type::identifier>
    {
        typedef identifier_token type;
        constexpr static char const* error_msg = "expected identifier";
    };

    template <>
    struct token_traits<token_type::integer_literal>
    {
        typedef integer_literal_token type;
        constexpr static char const* error_msg = "expected integer literal";
    };

    template <>
    struct token_traits<token_type::string_literal>
    {
        typedef string_literal_token type;
        constexpr static char const* error_msg = "expected string literal";
    };

    template <>
    struct token_traits<token_type::lbrace>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected '{'";
    };

    template <>
    struct token_traits<token_type::rbrace>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected '}'";
    };

    template <>
    struct token_traits<token_type::equals>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected '='";
    };

    template <>
    struct token_traits<token_type::semicolon>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected ';'";
    };

    template <>
    struct token_traits<token_type::comma>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected ','";
    };

    template <>
    struct token_traits<token_type::task_keyword>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected 'task' keyword";
    };

    template <>
    struct token_traits<token_type::true_keyword>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected 'true' keyword";
    };

    template <>
    struct token_traits<token_type::false_keyword>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected 'false' keyword";
    };

    template <>
    struct token_traits<token_type::depends_keyword>
    {
        typedef token type;
        constexpr static char const* error_msg = "expected 'depends' keyword";
    };

    struct parser_context : boost::noncopyable
    {
        parser_context(lexer& lex, error_tag_sink& esink)
            : lex(lex)
            , esink(esink)
            , last_error_place(nullptr)
        {}

        token_type tt()
        {
            if (lex.eof_token())
                return token_type::eof;
            return lex.peek_token().get_type();
        }

        void error(char const* error_message)
        {
            char const* pos;
            if (lex.eof_token())
                pos = lex.get_range().get_end();
            else
            {
                token& tok = lex.peek_token();
                pos = tok.get_range().get_start();
            }

            while (pos > lex.get_range().get_start() && is_ascii_whitespace(*(pos - 1)))
                --pos;

            if (last_error_place != pos)
            {
                esink.push(error_tag(text_range::make_empty(pos), error_message));
                last_error_place = pos;
            }
        }

        template <token_type type>
        std::unique_ptr<typename token_traits<type>::type> expect_token(char const* error_message)
        {
            typedef typename token_traits<type>::type token_t;
            typedef std::unique_ptr<token_t> token_ptr_t;

            if (tt() == type)
                return token_ptr_t(static_cast<token_t*>(lex.read_token().release()));
            else
            {
                error(error_message);
                return token_ptr_t();
            }
        }

        template <token_type type>
        std::unique_ptr<typename token_traits<type>::type> expect_token()
        {
            return expect_token<type>(token_traits<type>::error_msg);
        }

        template <token_type type>
        std::unique_ptr<typename token_traits<type>::type> assert_token()
        {
            typedef typename token_traits<type>::type token_t;
            typedef std::unique_ptr<token_t> token_ptr_t;

            token_sp t = lex.read_token();
            assert(t->get_type() == type);

            return std::move(token_ptr_t(static_cast<token_t*>(t.release())));
        }

        top_level_node_sp parse_file()
        {
            std::vector<task_node_sp> tasks;
            std::vector<dependency_node_sp> dependencies_decls;

            for (;;)
            {
                token_type t = tt();

                if (t == token_type::eof)
                {
                    break;
                }
                else if (t == token_type::task_keyword)
                {
                    if (task_node_sp n = parse_task())
                        tasks.push_back(std::move(n));
                }
                else if (t == token_type::identifier)
                {
                    dependencies_decls.push_back(parse_dependencies_declaration());
                }
                else
                {
                    error("expected declaration");
                    lex.read_token();
                }
            }

            return make_unique<top_level_node>(std::move(tasks), std::move(dependencies_decls));
        }

        task_node_sp parse_task()
        {
            token_sp            task_kw   = assert_token<token_type::task_keyword>();
            identifier_token_sp name      = expect_token<token_type::identifier>("expected task name");
            token_sp            eq        = expect_token<token_type::equals>();
            value_node_sp       value     = parse_value();
            token_sp            semicolon = expect_token<token_type::semicolon>();

            if (!name || !eq)
                return task_node_sp();

            return make_unique<task_node>(std::move(name), std::move(value));
        }

        value_node_sp parse_value()
        {
            token_type t = tt();
            if (t == token_type::integer_literal)
            {
                auto lit = assert_token<token_type::integer_literal>();
                return make_unique<integer_node>(std::move(lit));
            }
            else if (t == token_type::string_literal)
            {
                auto lit = assert_token<token_type::string_literal>();
                return make_unique<string_node>(std::move(lit));
            }
            else if (t == token_type::true_keyword)
            {
                auto lit = assert_token<token_type::true_keyword>();
                return make_unique<boolean_node>(std::move(lit), true);
            }
            else if (t == token_type::false_keyword)
            {
                auto lit = assert_token<token_type::false_keyword>();
                return make_unique<boolean_node>(std::move(lit), false);
            }
            else if (t == token_type::identifier)
            {
                identifier_token_sp tag = expect_token<token_type::identifier>("expected value tag");

                if (tt() != token_type::lbrace)
                    return make_unique<pseudo_identifier_value_node>(std::move(tag));

                token_sp lbrace = expect_token<token_type::lbrace>();

                std::vector<property_node_sp> properties;

                if (lbrace)
                {
                    for (;;)
                    {
                        token_type t = tt();
                        if (t == token_type::eof)
                            break;
                        if (t == token_type::rbrace)
                            break;
                        else if (t == token_type::identifier)
                        {
                            if (property_node_sp p = parse_property())
                                properties.push_back(std::move(p));

                            if (tt() != token_type::rbrace)
                                expect_token<token_type::comma>();
                        }
                        else
                        {
                            error("expected property");
                            lex.read_token();
                        }
                    }
                    token_sp rbrace = expect_token<token_type::rbrace>();

                    return make_unique<struct_node>(std::move(tag), std::move(properties));
                }
                else
                    return value_node_sp();
            }
            else
            {
                error("expected value");
                return value_node_sp();
            }
        }

        property_node_sp parse_property()
        {
            identifier_token_sp key  = assert_token<token_type::identifier>();
            token_sp eq              = expect_token<token_type::equals>();
            value_node_sp value      = parse_value();

            if (!eq)
                return property_node_sp();

            return make_unique<property_node>(std::move(key), std::move(eq), std::move(value));
        }

        dependency_node_sp parse_dependencies_declaration()
        {
            identifier_token_sp name       = assert_token<token_type::identifier>();
            token_sp            depends_kw = expect_token<token_type::depends_keyword>();

            std::vector<identifier_token_sp> dependencies;

            for (;;)
            {
                identifier_token_sp id = expect_token<token_type::identifier>();
                if (id)
                    dependencies.push_back(std::move(id));

                token_type t = tt();
                if (t == token_type::eof)
                    break;
                else if (t == token_type::semicolon)
                    break;
                else if (t == token_type::comma)
                    lex.read_token();
                else
                {
                    if (!id)
                        break;
                    error("expected ',' or ';'");
                }
            }

            expect_token<token_type::semicolon>();

            return make_unique<dependency_node>(std::move(name), std::move(dependencies));
        }

    private:
        lexer& lex;
        error_tag_sink& esink;
        char const* last_error_place;
    };
}

top_level_node_sp parse_file(lexer& lex, error_tag_sink& esink)
{
    parser_context pctx(lex, esink);

    return pctx.parse_file();
}
