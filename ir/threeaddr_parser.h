#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#define TOKEN_TYPE(t) (t.type)
enum TokenType {
        VARIABLE,
        INSTRUCTION_ALLOCA,
        INSTRUCTION_ADD,
        INSTRUCTION_SUB,
        INSTRUCTION_MUL,
        INSTRUCTION_DIV,
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
        STR,
        COLON,
        AT,
        LCURLY,
        RCURLY,
        END
};

struct Token {
        enum TokenType type;
        union {
                int value;
                char fn_name[MAX_IDENTIFIER_LEN + 1];
        };
};

struct Token Token (enum TokenType type, int value);
char peek_char ();
char advance_char ();
bool match_char (char c);
bool match_str (char *s);
bool is_numeric (char c);
bool is_alpha (char c);
int parse_int (int lead_digit);
void parse_str (char lead_char, char *buffer);
struct Token next_token ();
struct Token advance_token ();
bool match_token (enum TokenType t);
struct Token peek_token ();
struct Token consume_token (enum TokenType t, char *error_message, ...);
void threeaddr_init_parser (char *ir);
