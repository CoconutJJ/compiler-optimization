#pragma once
#include "global_constants.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOKEN_TYPE(t) ((t).type)
enum TokenType {
        VARIABLE,
        INSTRUCTION_ALLOCA,
        INSTRUCTION_ADD,
        INSTRUCTION_CMP,
        INSTRUCTION_SUB,
        INSTRUCTION_MUL,
        INSTRUCTION_DIV,
        INSTRUCTION_XOR,
        INSTRUCTION_STORE,
        INSTRUCTION_LOAD,
        INSTRUCTION_JUMP,
        INSTRUCTION_JUMPIF,
        INSTRUCTION_PHI,
        COMMA,
        FN,
        LPAREN,
        RPAREN,
        INTEGER,
        LABEL,
        LABEL_LITERAL,
        STR,
        COLON,
        AT,
        LCURLY,
        RCURLY,
        END,
        NIL
};

struct Token {
        enum TokenType type;
        size_t line_number;
        size_t column_number;
        char *line;
        union {
                int value;
                char str_value[MAX_IDENTIFIER_LEN + 1];
        };
};
struct Token Token (enum TokenType type, int value);
struct Token next_token (void);
struct Token advance_token (void);
bool match_token (enum TokenType t);
struct Token peek_token (void);
struct Token consume_token (enum TokenType t, char *error_message, ...);
void threeaddr_init_parser (char *ir);
void error (struct Token target, char *message, ...);
void _va_error (struct Token target, char *title, char *annotation, va_list args);