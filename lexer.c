#include "lexer.h"
#include <stdbool.h>
#include <string.h>

static inline bool is_alpha(const char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c & 0b10000000))
    {
        return true;
    }

    return false;
}

static inline bool is_digit(const char c)
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }

    return false;
}

static inline bool is_whitespace(const char c)
{
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
    {
        return true;
    }

    return false;
}

Lexer lexer_create(FILE *input_file)
{
    char *line_mem = malloc(255);

    Lexer lexer = {
        .input_file = input_file,
        .next_line = line_mem,
        .p = "\0",
    };

    return lexer;
}

static void lexer_fatal(const char *error_msg)
{
    fprintf(stderr, "Lexer Fatal: %s", error_msg);

    exit(-1);
}

static void fetch_next_line(Lexer *lexer)
{
    if (fgets(lexer->next_line, 255, lexer->input_file) == NULL)
    {
        lexer->next_line = "\0";
    }

    lexer->p = lexer->next_line;
}

static TokenId get_keyword(const char *text, size_t l)
{
    if (strncmp(text, "fn", l) == 0) // TODO: hashmap
    {
        return TOK_FN;
    }

    return TOK_EOF;
}

Token lexer_next(Lexer *lexer)
{
    for (;;)
    {
        while (*lexer->p == '\0')
        {
            if (feof(lexer->input_file))
            {
                return (Token){0};
            }

            fetch_next_line(lexer);
        }
        Token next = {0};

        if (is_alpha(*lexer->p)) // lex identifier and reserved keywords
        {
            const char *start = lexer->p;

            do
            {
                ++lexer->p;
            } while (is_alpha(*lexer->p));

            const size_t l = lexer->p - start;

            const TokenId keyword = get_keyword(start, l);
            if (keyword == TOK_EOF)
            {
                next.id = TOK_STRING;
                next.text = malloc(l + 1);
                next.text[l] = '\0';

                memcpy(next.text, start, l);
            }
            else
            {
                next.id = keyword;
            }

            return next;
        }
        else if (is_digit(*lexer->p)) // lex integers
        {
            const char *start = lexer->p;

            do
            {
                ++lexer->p;
            } while (is_digit(*lexer->p));

            const size_t l = lexer->p - start;

            next.id = TOK_INT;
            next.text = malloc(l + 1);
            next.text[l] = '\0';

            memcpy(next.text, start, l);

            return next;
        }
        else if (is_whitespace(*lexer->p)) // lex whitespace and line breaks
        {
            lexer->p++;
        }
        else // lex short token
        {
            switch (*lexer->p)
            {
            case '(':
                next.id = TOK_BRACKET_CURVY_OPEN;
                break;
            case ')':
                next.id = TOK_BRACKET_CURVY_CLOSE;
                break;
            case '{':
                next.id = TOK_BRACKET_CURLY_OPEN;
                break;
            case '}':
                next.id = TOK_BRACKET_CURLY_CLOSE;
                break;
            case '[':
                next.id = TOK_BRACKET_BOX_OPEN;
                break;
            case ']':
                next.id = TOK_BRACKET_BOX_CLOSE;
                break;
            case '+':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_ARITH_PLUS;
                }
                else
                {
                    next.id = TOK_ARITH_PLUS;
                }
                break;
            case '-':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_ARITH_MINUS;
                }
                else
                {
                    next.id = TOK_ARITH_MINUS;
                }
                break;
            case '*':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_ARITH_MULTIPLY;
                }
                else
                {
                    next.id = TOK_ARITH_MULTIPLY;
                }
                break;
            case '/':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_ARITH_DIVIDE;
                }
                else
                {
                    next.id = TOK_ARITH_DIVIDE;
                }
                break;
            case '%':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_ARITH_MODULO;
                }
                else
                {
                    next.id = TOK_ARITH_MODULO;
                }
                break;
            case '=':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_COMP_EQUAL;
                }
                else
                {
                    next.id = TOK_ASSIGN;
                }
                break;
            case '&':
                if (*(lexer->p + 1) == '&')
                {
                    lexer->p++;
                    next.id = TOK_LOGIC_AND;
                }
                else if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_BIT_AND;
                }
                else
                {
                    next.id = TOK_BIT_AND;
                }
                break;
            case '|':
                if (*(lexer->p + 1) == '|')
                {
                    lexer->p++;
                    next.id = TOK_LOGIC_OR;
                }
                else if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_BIT_OR;
                }
                else
                {
                    next.id = TOK_BIT_OR;
                }
                break;
            case '^':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_ASSIGN_BIT_XOR;
                }
                else
                {
                    next.id = TOK_BIT_XOR;
                }
                break;
            case '~':
                next.id = TOK_BIT_NEGATE;
                break;
            case '<':
                if (*(lexer->p + 1) == '<')
                {
                    lexer->p++;

                    if (*(lexer->p + 1) == '=')
                    {
                        lexer->p++;
                        next.id = TOK_ASSIGN_BIT_SHIFT_LEFT;
                    }
                    else
                    {
                        next.id = TOK_BIT_SHIFT_LEFT;
                    }
                }
                else if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_COMP_SMALLER_EQ;
                }
                else
                {
                    next.id = TOK_COMP_SMALLER;
                }
                break;
            case '>':
                if (*(lexer->p + 1) == '>')
                {
                    lexer->p++;

                    if (*(lexer->p + 1) == '=')
                    {
                        lexer->p++;
                        next.id = TOK_ASSIGN_BIT_SHIFT_RIGHT;
                    }
                    else if (*(lexer->p + 1) == '=')
                    {
                        lexer->p++;
                        next.id = TOK_COMP_LARGER_EQ;
                    }
                    else
                    {
                        next.id = TOK_BIT_SHIFT_RIGHT;
                    }
                }
                else if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_COMP_LARGER_EQ;
                }
                else
                {
                    next.id = TOK_COMP_LARGER;
                }
                break;
            case '!':
                if (*(lexer->p + 1) == '=')
                {
                    lexer->p++;
                    next.id = TOK_COMP_UNEQUAL;
                }
                else
                {
                    next.id = TOK_LOGIC_NOT;
                }
                break;
            default:
                lexer_fatal("Unknown symbol");
                break;
            }

            lexer->p++;

            return next;
        }
    }
}
