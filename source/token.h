#ifndef CPPLNS_TOKEN_H
#define CPPLNS_TOKEN_H

#include <string>
#include <sstream>
#include "primitives.h"

namespace lns {
    enum token_type {
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_SQR,
        RIGHT_SQR,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        COLON,
        MINUS,
        MINUS_EQUALS,
        PLUS,
        PLUS_EQUALS,
        PLUS_PLUS,
        MINUS_MINUS,
        SLASH,
        SLASH_EQUALS,
        STAR,
        STAR_EQUALS,
        HAT,
        MODULO,
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        IDENTIFIER,
        STRING,
        NUMBER,
        NATIVES,
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUNCTION,
        FOR,
        FOREACH,
        IF,
        NUL,
        OR,
        XOR,
        NOR,
        NAND,
        NOT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
        GLOBAL,
        LOCAL,
        FINAL,
        USE,
        BREAK,
        CONTINUE,
        CONTEXT,
        BEGIN,
        THEN,
        DO,
        END,
        DPCHECK,
        IN,
        BIND,
        EXCEPTION,
        WITH,
        RAISE,
        HANDLE,
        CONSTRUCTOR,
        UNRECOGNIZED,
        QUESTION_MARK,
        NEW,
        EXTENDS,
        EOF_
    };


    enum operator_location {
        POSTFIX, PREFIX
    };

    enum visibility {
        V_GLOBAL, V_LOCAL, V_UNSPECIFIED
    };

    const std::string KEYWORDS_S_VALUES[] = {"COMMA", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_SQR", "RIGHT_SQR",
                                             "LEFT_BRACE", "RIGHT_BRACE", "COMMA", "DOT", "COLON", "MINUS",
                                             "MINUS_EQUALS", "PLUS", "PLUS_EQUALS", "PLUS_PLUS", "MINUS_MINUS",
                                             "SLASH", "SLASH_EQUALS", "STAR", "STAR_EQUALS", "HAT", "MODULO", "BANG",
                                             "BANG_EQUAL",
                                             "EQUAL", "EQUAL_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",
                                             "IDENTIFIER", "STRING", "NUMBER", "NATIVES", "AND", "CLASS", "ELSE",
                                             "FALSE",
                                             "FUNCTION", "FOR", "FOREACH", "IF", "NUL", "OR", "XOR", "NOR", "NAND",
                                             "NOT", "RETURN",
                                             "SUPER", "THIS", "TRUE", "VAR", "WHILE", "GLOBAL", "LOCAL", "FINAL", "USE",
                                             "BREAK",
                                             "CONTINUE", "CONTEXT", "BEGIN", "THEN", "DO", "END", "DPCHECK", "IN",
                                             "BIND", "EXCEPTION", "WITH", "RAISE", "HANDLE", "CONSTRUCTOR",
                                             "UNRECOGNIZED", "QUESTION_MARK",
                                             "NEW", "EXTENDS", "EOF_"};

    class token {
    public:
        token(token_type type, std::string lexeme, std::shared_ptr<object> literal, const char *filename, int line);

        token_type type;
        std::string lexeme;
        std::shared_ptr<object> literal;
        const char *filename;
        int line;

        std::string format() const;
        //void operator delete(void*){}
    };

}
#endif //CPPLNS_TOKEN_H
