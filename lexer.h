#ifndef _UWU_LEXER_H_
#define _UWU_LEXER_H_

#include <stdlib.h>
#include <stdio.h>
#include "token.h"

/**
 * @brief The lexer takes a stream of characters as
 *        input and converts them into a stream of @ref Token.
 */
typedef struct
{
    FILE *input_file;
    char *line_buffer;       // current line
    size_t line_buffer_size; // size of line_buffer
    char *p;                 // current position in line_buffer
} Lexer;

/**
 * @brief Init lexer and allocate line buffer
 */
Lexer lexer_create(FILE *input_file);

/**
 * @brief Fetch next @ref Token from lexer input
 */
Token lexer_next(Lexer *lexer);

#endif // _UWU_LEXER_H_
