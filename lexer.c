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

    exit(1);
}

/**
 * @brief Load next line from input file into line_buffer.
 *
 * @return returns false if EOF has been reached, true otherwise
 */
static bool lexer_fetch_line(Lexer *lexer)
{
    size_t line_buffer_pos = 0;
    for (int next_char;;)
    {
        next_char = fgetc(lexer->input_file);

        if (next_char == EOF)
        {
            return false;
        }

        if (next_char == '\n')
        {
            break;
        }

        if (line_buffer_pos >= lexer->line_buffer_size)
        {
            lexer->line_buffer_size *= 2;
            lexer->line_buffer = realloc(lexer->line_buffer, lexer->line_buffer_size);
        }

        lexer->line_buffer[line_buffer_pos++] = (char)next_char;
    }

    lexer->line_buffer[line_buffer_pos] = '\0';
    lexer->p = lexer->line_buffer;

    return true;
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

    return next;
}

/*
 *  ------------------------------------
 *  public lexer functions
 *  ------------------------------------
 */

Lexer lexer_create(FILE *input_file)
{
    const size_t initial_size = 32;
    char *line_buffer = malloc(initial_size);

    Lexer lexer = {
        .input_file = input_file,
        .line_buffer = line_buffer,
        .line_buffer_size = initial_size,
        .p = "\0",
    };

    return lexer;
}

Token lexer_next(Lexer *lexer)
{
    for (;;)
    {
        while (*lexer->p == '\0') // end of line
        {
            if (!lexer_fetch_line(lexer)) // fetch and check for EOF
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
            ++lexer->p; // skip whitespace
        }
        else
        {
            return lexer_lex_symbol(lexer);
        }
    }
}
