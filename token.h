#ifndef _UWU_TOKEN_H_
#define _UWU_TOKEN_H_

typedef enum
{
    TOK_EOF = 0, // end of input file

    TOK_FN, // 'fn' - function definition

    TOK_STRING, // string
    TOK_INT,    // integer
    TOK_FLOAT,  // float

    TOK_BRACKET_CURVY_OPEN,  // (
    TOK_BRACKET_CURVY_CLOSE, // )
    TOK_BRACKET_CURLY_OPEN,  // {
    TOK_BRACKET_CURLY_CLOSE, // }
    TOK_BRACKET_BOX_OPEN,    // [
    TOK_BRACKET_BOX_CLOSE,   // ]

    TOK_ARITH_PLUS,     // +
    TOK_ARITH_MINUS,    // -
    TOK_ARITH_MULTIPLY, // *
    TOK_ARITH_DIVIDE,   // /
    TOK_ARITH_MODULO,   // %

    TOK_ASSIGN,                // =

    TOK_ASSIGN_ARITH_PLUS,     // +=
    TOK_ASSIGN_ARITH_MINUS,    // -=
    TOK_ASSIGN_ARITH_MULTIPLY, // *=
    TOK_ASSIGN_ARITH_DIVIDE,   // /=
    TOK_ASSIGN_ARITH_MODULO,   // %=

    TOK_BIT_AND,         // &
    TOK_BIT_OR,          // |
    TOK_BIT_XOR,         // ^
    TOK_BIT_NEGATE,      // ~
    TOK_BIT_SHIFT_LEFT,  // <<
    TOK_BIT_SHIFT_RIGHT, // >>

    TOK_ASSIGN_BIT_AND,         // &=
    TOK_ASSIGN_BIT_OR,          // |=
    TOK_ASSIGN_BIT_XOR,         // ^=
    TOK_ASSIGN_BIT_SHIFT_LEFT,  // <<=
    TOK_ASSIGN_BIT_SHIFT_RIGHT, // >>=

    TOK_LOGIC_AND, // &&
    TOK_LOGIC_OR,  // ||
    TOK_LOGIC_NOT, // !

    TOK_COMP_EQUAL,      // ==
    TOK_COMP_UNEQUAL,    // !=
    TOK_COMP_SMALLER,    // <
    TOK_COMP_LARGER,     // >
    TOK_COMP_SMALLER_EQ, // <=
    TOK_COMP_LARGER_EQ,  // >=

} TokenId;

typedef struct
{
    TokenId id;

    union
    {
        char *text;
    };
} Token;

const char *token_name(const TokenId id);

#endif // _UWU_TOKEN_H_