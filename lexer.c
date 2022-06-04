#include "lexer.h"
#include <stdbool.h>
#include <string.h>

/*
 *  ------------------------------------
 *  lexer helper functions
 *  ------------------------------------
 */

/**
 * @brief matches a-z, A-Z and any unicode character
 */
static inline bool is_alpha(const char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c & 0b10000000))
    {
        return true;
    }

    return false;
}

/**
 * @brief matches 0-9
 */
static inline bool is_digit(const char c)
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }

    return false;
}

/**
 * @brief matches ascii whitespace, CR/LF and tab
 */
static inline bool is_whitespace(const char c)
{
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
    {
        return true;
    }

    return false;
}

/**
 * @brief Compares text with reserved keywords and returns
 *        related @ref TokenId if a keyword has been matched
 */
static inline TokenId find_keyword(const char *text, const size_t l)
{
    if (strncmp(text, "fn", l) == 0)
    {
        return TOK_FN;
    }

    return TOK_EOF;
}

/*
 *  ------------------------------------
 *  static lexer functions
 *  ------------------------------------
 */

/**
 * @brief Print error message and exit application
 */
static void lexer_fatal(const char *error_msg)
{
    fprintf(stderr, "Lexer Fatal: %s", error_msg);

    exit(-1);
}

/**
 * @brief Load next line from input file into next_line buffer.
 */
static void lexer_fetch_line(Lexer *lexer)
{
    if (fgets(lexer->next_line, 255, lexer->input_file) == NULL) // TODO: increase size dynamically
    {
        lexer->next_line = "\0";
    }

    lexer->p = lexer->next_line;
}

/**
 * @brief Lexes a name or identifier
 */
static inline Token lexer_lex_word(Lexer *lexer)
{
    Token next = {0};

    const char *start = lexer->p;

    do
    {
        ++lexer->p;
    } while (is_alpha(*lexer->p));

    const size_t l = lexer->p - start;

    // Check if the name is a reserved keyword
    const TokenId keyword = find_keyword(start, l);
    if (keyword == TOK_EOF)
    {
        next.id = TOK_NAME;
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

/**
 * @brief Lexes an integer
 */
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

/**
 * @brief Lexes a bracket and operator
 */
static inline Token lexer_lex_symbol(Lexer *lexer)
{
    Token next = {0};

    switch (*(lexer->p++))
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
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_ARITH_PLUS;
        }
        else
        {
            next.id = TOK_ARITH_PLUS;
        }
        break;
    case '-':
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_ARITH_MINUS;
        }
        else
        {
            next.id = TOK_ARITH_MINUS;
        }
        break;
    case '*':
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_ARITH_MULTIPLY;
        }
        else
        {
            next.id = TOK_ARITH_MULTIPLY;
        }
        break;
    case '/':
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_ARITH_DIVIDE;
        }
        else
        {
            next.id = TOK_ARITH_DIVIDE;
        }
        break;
    case '%':
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_ARITH_MODULO;
        }
        else
        {
            next.id = TOK_ARITH_MODULO;
        }
        break;
    case '=':
        if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_COMP_EQUAL;
        }
        else
        {
            next.id = TOK_ASSIGN;
        }
        break;
    case '&':
        if (*lexer->p == '&')
        {
            ++lexer->p;
            next.id = TOK_LOGIC_AND;
        }
        else if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_BIT_AND;
        }
        else
        {
            next.id = TOK_BIT_AND;
        }
        break;
    case '|':
        if (*lexer->p == '|')
        {
            ++lexer->p;
            next.id = TOK_LOGIC_OR;
        }
        else if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_ASSIGN_BIT_OR;
        }
        else
        {
            next.id = TOK_BIT_OR;
        }
        break;
    case '^':
        if (*lexer->p == '=')
        {
            ++lexer->p;
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
        if (*lexer->p == '<')
        {
            ++lexer->p;

            if (*lexer->p == '=')
            {
                ++lexer->p;
                next.id = TOK_ASSIGN_BIT_SHIFT_LEFT;
            }
            else
            {
                next.id = TOK_BIT_SHIFT_LEFT;
            }
        }
        else if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_COMP_SMALLER_EQ;
        }
        else
        {
            next.id = TOK_COMP_SMALLER;
        }
        break;
    case '>':
        if (*lexer->p == '>')
        {
            ++lexer->p;

            if (*lexer->p == '=')
            {
                ++lexer->p;
                next.id = TOK_ASSIGN_BIT_SHIFT_RIGHT;
            }
            else
            {
                next.id = TOK_BIT_SHIFT_RIGHT;
            }
        }
        else if (*lexer->p == '=')
        {
            ++lexer->p;
            next.id = TOK_COMP_LARGER_EQ;
        }
        else
        {
            next.id = TOK_COMP_LARGER;
        }
        break;
    case '!':
        if (*lexer->p == '=')
        {
            ++lexer->p;
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

    ++lexer->p;

    return next;
}

/*
 *  ------------------------------------
 *  public lexer functions
 *  ------------------------------------
 */

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

Token lexer_next(Lexer *lexer)
{
    for (;;)
    {
        if (*lexer->p == '\0') // end of line
        {
            lexer_fetch_line(lexer);

            if (feof(lexer->input_file)) // end of input file
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
            ++lexer->p; // ignore whitespace
        }
        else
        {
            return lexer_lex_symbol(lexer);
        }
    }
}
