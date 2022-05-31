#ifndef _UWU_LEXER_H_
#define _UWU_LEXER_H_

#include <stdlib.h>
#include <stdio.h>
#include "token.h"

typedef struct
{
    FILE *input_file;
    char *next_line;
    char *p;
} Lexer;

Lexer lexer_create();
Token lexer_next(Lexer *lexer);

#endif // _UWU_LEXER_H_
