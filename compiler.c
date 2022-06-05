#include "lexer.h"

int main(int argc, char *argv[])
{
    FILE *input_file = fopen("lex-test.uwu", "r");

    if (input_file == NULL)
    {
        fprintf(stderr, "Failed to open input file");

        exit(-1);
    }

    Lexer lexer = lexer_create(input_file);

    Token next;
    while ((next = lexer_next(&lexer)).id != TOK_EOF)
    {
        printf("%d %s\n", next.id, token_name(next.id));

        if(next.text) {
            free(next.text);
        }
    }

    return 0;
}
