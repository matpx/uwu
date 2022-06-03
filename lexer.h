#ifndef _UWU_LEXER_H_
#define _UWU_LEXER_H_

#include <stdlib.h>
#include <stdio.h>
#include "token.h"

typedef struct
{
    FILE *input_file;
    char *next_line; // line buffer
    char *p;         // current position in next_line
} Lexer;

/**
 * @brief Init lexer and allocate line buffer
 *
 * @param input_file Open input file
 * @return Lexer
 */
Lexer lexer_create();

/**
 * @brief Fetch next @ref Token from lexer input
 *
 * @param lexer
 * @return Token Next @ref Token from input
 */
Token lexer_next(Lexer *lexer);

#endif // _UWU_LEXER_H_
