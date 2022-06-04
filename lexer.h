#ifndef _UWU_LEXER_H_
#define _UWU_LEXER_H_

#include <stdlib.h>
#include <stdio.h>
#include "token.h"

/**
 * @brief The lexer takes a stream of characters as
 * input and converts them into a stream of @ref Token.
 */
typedef struct
{
    FILE *input_file;
    char *next_line; // line buffer
    char *p;         // current position in next_line
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
