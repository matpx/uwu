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

static inline Token lexer_lex_word(Lexer *lexer)
{
    Token next = {0};

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

static inline Token lexer_lex_number(Lexer *lexer)
{
    Token next = {0};

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

#define IF_ASSIGN(ASSIGN_TOK, ELSE_TOK) \
    if (*(lexer->p + 1) == '=')         \
    {                                   \
        lexer->p++;                     \
        next.id = ASSIGN_TOK;           \
    }                                   \
    else                                \
    {                                   \
        next.id = ELSE_TOK;             \
    }                                   \
    break;

#define IF_LOGIC_OR_BIT(SECOND_SYMBOL, LOGIC_TOK, ASSIGN_BIT_TOK, BIT_TOK) \
    if (*(lexer->p + 1) == SECOND_SYMBOL)                                  \
    {                                                                      \
        lexer->p++;                                                        \
        next.id = LOGIC_TOK;                                               \
    }                                                                      \
    else if (*(lexer->p + 1) == '=')                                       \
    {                                                                      \
        lexer->p++;                                                        \
        next.id = ASSIGN_BIT_TOK;                                          \
    }                                                                      \
    else                                                                   \
    {                                                                      \
        next.id = BIT_TOK;                                                 \
    }                                                                      \
    break;

#define IF_BIT_OR_COMP(SECOND_SYMBOL, ASSIGN_BIT_TOK, BIT_TOK, COMP_EQ_TOK, COMP_TOK) \
    if (*(lexer->p + 1) == SECOND_SYMBOL)                                             \
    {                                                                                 \
        lexer->p++;                                                                   \
                                                                                      \
        if (*(lexer->p + 1) == '=')                                                   \
        {                                                                             \
            lexer->p++;                                                               \
            next.id = ASSIGN_BIT_TOK;                                                 \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            next.id = BIT_TOK;                                                        \
        }                                                                             \
    }                                                                                 \
    else if (*(lexer->p + 1) == '=')                                                  \
    {                                                                                 \
        lexer->p++;                                                                   \
        next.id = COMP_EQ_TOK;                                                        \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        next.id = COMP_TOK;                                                           \
    }                                                                                 \
    break;

static inline Token lexer_lex_symbol(Lexer *lexer)
{
    Token next = {0};

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
        IF_ASSIGN(TOK_ASSIGN_ARITH_PLUS, TOK_ARITH_PLUS);
    case '-':
        IF_ASSIGN(TOK_ASSIGN_ARITH_MINUS, TOK_ARITH_MINUS);
    case '*':
        IF_ASSIGN(TOK_ASSIGN_ARITH_MULTIPLY, TOK_ARITH_MULTIPLY);
    case '/':
        IF_ASSIGN(TOK_ASSIGN_ARITH_DIVIDE, TOK_ARITH_DIVIDE);
    case '%':
        IF_ASSIGN(TOK_ASSIGN_ARITH_MODULO, TOK_ARITH_MODULO);
    case '=':
        IF_ASSIGN(TOK_COMP_EQUAL, TOK_ASSIGN);
    case '&':
        IF_LOGIC_OR_BIT('&', TOK_LOGIC_AND, TOK_ASSIGN_BIT_AND, TOK_BIT_AND);
    case '|':
        IF_LOGIC_OR_BIT('|', TOK_LOGIC_OR, TOK_ASSIGN_BIT_OR, TOK_BIT_OR);
    case '^':
        IF_ASSIGN(TOK_ASSIGN_BIT_XOR, TOK_BIT_XOR);
    case '~':
        next.id = TOK_BIT_NEGATE;
        break;
    case '<':
        IF_BIT_OR_COMP('<', TOK_ASSIGN_BIT_SHIFT_LEFT, TOK_BIT_SHIFT_LEFT, TOK_COMP_SMALLER_EQ, TOK_COMP_SMALLER);
    case '>':
        IF_BIT_OR_COMP('>', TOK_ASSIGN_BIT_SHIFT_RIGHT, TOK_BIT_SHIFT_RIGHT, TOK_COMP_LARGER_EQ, TOK_COMP_LARGER);
    case '!':
        IF_ASSIGN(TOK_COMP_UNEQUAL, TOK_LOGIC_NOT);
    default:
        lexer_fatal("Unknown symbol");
        break;
    }

    lexer->p++;

    return next;
}

Token lexer_next(Lexer *lexer)
{
    for (;;)
    {
        if (*lexer->p == '\0')
        {
            fetch_next_line(lexer);

            if (feof(lexer->input_file))
            {
                return (Token){0};
            }
        }

        if (is_alpha(*lexer->p))
        {
            return lexer_lex_word(lexer);
        }
        else if (is_digit(*lexer->p))
        {
            return lexer_lex_number(lexer);
        }
        else if (is_whitespace(*lexer->p))
        {
            lexer->p++;
        }
        else
        {
            return lexer_lex_symbol(lexer);
        }
    }
}
