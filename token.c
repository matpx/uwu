#include "token.h"

const char *token_name(const TokenId id)
{
    switch (id)
    {
    case TOK_EOF:
        return "Invalid: End of File";
    case TOK_FN:
        return "fn";
    case TOK_STRING:
        return "string";
    case TOK_INT:
        return "integer";
    case TOK_FLOAT:
        return "float";
    case TOK_BRACKET_CURVY_OPEN:
        return "(";
    case TOK_BRACKET_CURVY_CLOSE:
        return ")";
    case TOK_BRACKET_CURLY_OPEN:
        return "{";
    case TOK_BRACKET_CURLY_CLOSE:
        return "}";
    case TOK_BRACKET_BOX_OPEN:
        return "[";
    case TOK_BRACKET_BOX_CLOSE:
        return "]";
    case TOK_ARITH_PLUS:
        return "+";
    case TOK_ARITH_MINUS:
        return "-";
    case TOK_ARITH_MULTIPLY:
        return "*";
    case TOK_ARITH_DIVIDE:
        return "/";
    case TOK_ARITH_MODULO:
        return "%";
    case TOK_ASSIGN:
        return "=";
    case TOK_ASSIGN_ARITH_PLUS:
        return "+=";
    case TOK_ASSIGN_ARITH_MINUS:
        return "-=";
    case TOK_ASSIGN_ARITH_MULTIPLY:
        return "*=";
    case TOK_ASSIGN_ARITH_DIVIDE:
        return "/=";
    case TOK_ASSIGN_ARITH_MODULO:
        return "%=";
    case TOK_BIT_AND:
        return "&";
    case TOK_BIT_OR:
        return "|";
    case TOK_BIT_XOR:
        return "^";
    case TOK_BIT_NEGATE:
        return "~";
    case TOK_BIT_SHIFT_LEFT:
        return "<<";
    case TOK_BIT_SHIFT_RIGHT:
        return ">>";
    case TOK_ASSIGN_BIT_AND:
        return "&=";
    case TOK_ASSIGN_BIT_OR:
        return "|=";
    case TOK_ASSIGN_BIT_XOR:
        return "^=";
    case TOK_ASSIGN_BIT_SHIFT_LEFT:
        return "<<=";
    case TOK_ASSIGN_BIT_SHIFT_RIGHT:
        return ">>=";
    case TOK_LOGIC_AND:
        return "&&";
    case TOK_LOGIC_OR:
        return "||";
    case TOK_LOGIC_NOT:
        return "!";
    case TOK_COMP_EQUAL:
        return "==";
    case TOK_COMP_UNEQUAL:
        return "!=";
    case TOK_COMP_SMALLER:
        return "<";
    case TOK_COMP_LARGER:
        return ">";
    case TOK_COMP_SMALLER_EQ:
        return "<=";
    case TOK_COMP_LARGER_EQ:
        return ">=";
    }

    return 0;
}
