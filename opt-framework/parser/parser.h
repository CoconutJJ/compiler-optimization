#pragma once

#define MAX_IDENTIFIER_LEN 20
#define MAX_LITERAL_LEN 20

enum TokenType {
    IDENTIFIER,
    ADD,
    MINUS,
    MULT,
    DIV,
    PHI,
    EQ,
    LITERAL,
    COMMA,
    COLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    JUMPIF,
    JUMP,
    END_OF_FILE
};

typedef struct Token {
    enum TokenType type;
    union {
        double dvalue;
        long ivalue;
        char svalue[20];
    };
} Token;

Token parse_token ();



