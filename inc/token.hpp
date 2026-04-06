#pragma once
#include "common.hpp"
#undef NULL
#undef EOF

namespace Brass
{
    enum class TokenType {
        // Grouping
        LPAREN, RPAREN,
        LBRACK, RBRACK,
        LBRACE, RBRACE,

        // Unary
        PLUS_PLUS, MINUS_MINUS,

        // Arithmetic Operators
        PLUS, PLUS_EQUAL,
        MINUS, MINUS_EQUAL,
        STAR, STAR_EQUAL,
        SLASH, SLASH_EQUAL,
        MODULO, MODULO_EQUAL,
        ASSIGN,
        
        // Bitwise Operators
        BIN_AND, BIN_AND_EQUAL,
        BIN_OR, BIN_OR_EQUAL,
        XOR, XOR_EQUAL,
        BIN_NOT, BIN_NOT_EQUAL,
        SHL, SHL_EQUAL,
        SHR, SHR_EQUAL,

        // Comparative Operators
        LESS, LESS_EQUAL,
        GREATER, GREATER_EQUAL,
        EQUALS, BANG_EQUALS,

        // Boolean Operators
        BANG, LOG_AND, LOG_OR,

        // Miscellaneous
        COMMA, DOT,
        ARROW, SEMICOLON,
        DOUBLE_COLON, QUESTION,
        COLON,

        // Keyword Operators
        // AND WILL ALWAYS BE FIRST
        AND, OR, IS,
        SIZEOF, TYPEOF,
        IN,

        // Types
        U8, S8, U16, S16,
        U32, S32, U64, S64,
        F32, F64,

        // Aliases
        CHAR, BOOL, BYTE,
        INT, UINT, LONG,
        ULONG, FLOAT, DOUBLE,

        // Custom Types
        TYPE, ENUM, EXCEPTION,

        // Declarations
        VAR, FN, SELF,
        IMPL, OPERATOR,
        USING, FROM,

        // Control
        DO, WHILE, FOR,
        FOREACH, TIMES,
        SWITCH, BREAK,
        CONTINUE, RETURN,
        CASE, DEFAULT,
        IF, ELIF, ELSE,

        // Visibility
        PUBLIC, PROTECTED,
        INTERNAL, LOCAL,
        PRIVATE,

        // Function Modifiers
        CDECL, EXTERN, ABSTRACT,
        PURE, SEALED, INLINE,
        OVERRIDE,

        // Variable Modifiers
        UNMANAGED, VOLATILE,
        MUT, COMPTIME,

        // Type Modifiers
        PRIMITIVE,

        // Memory
        NEW, DELETE, 
        NULL, REINTERPRET,

        // Exceptions
        TRY, CATCH, FINALLY,
        THROW, THROWS,
        // THROWS WILL ALWAYS BE LAST

        // Literals
        IDENTIFIER, STRING_LITERAL,
        INT_LITERAL, FLOAT_LITERAL,
        INTERPOLATED_STRING,

        UNKNOWN, EOF
    };

    struct Token
    {
        TokenType type;
        size_t start;
        size_t length;
        
        string file;
        int line;
        int column;

        operator TokenType() const { return type; }
    };

    TokenType getTokenFromString(string str);
    string getStringFromToken(TokenType type);
} // namespace Brass