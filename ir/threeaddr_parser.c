#include "threeaddr_parser.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
static char *ir_source = NULL;
static size_t ir_source_index = 0;

static struct Token curr_token;

struct Token Token (enum TokenType type, int value)
{
        return (struct Token){ .type = type, .value = value };
}

char peek_char ()
{
        return ir_source[ir_source_index];
}

char advance_char ()
{
        char old = ir_source[ir_source_index];
        if (peek_char () != '\0')
                ir_source_index++;

        return old;
}

bool match_char (char c)
{
        if (peek_char () == c) {
                ir_source_index++;
                return true;
        }
        return false;
}

bool match_str (char *s)
{
        if (strncmp (s, ir_source + ir_source_index, strlen (s)) == 0) {
                ir_source_index += strlen (s);
                return true;
        }

        return false;
}

bool is_numeric (char c)
{
        return ('0' <= c && c <= '9');
}

bool is_alpha (char c)
{
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

int parse_int (int lead_digit)
{
        char c;

        int value = lead_digit;
        while (is_numeric ((c = peek_char ()))) {
                value = 10 * value + (c - '0');
                advance_char ();
        }

        return value;
}

void parse_str (char lead_char, char *buffer)
{
        buffer[0] = lead_char;

        int i = 1;

        char c;
        while (is_alpha (c = peek_char ()) || is_numeric (c)) {
                buffer[i++] = advance_char();

                if (i >= MAX_IDENTIFIER_LEN) {
                        fprintf (stderr, "Identifier exceeds maximum identifier length of %d", MAX_IDENTIFIER_LEN);
                        exit (EXIT_FAILURE);
                }
        }

        buffer[i] = '\0';
}

struct Token next_token ()
{
        for (;;) {
                char c = advance_char ();

                if (c == '\0')
                        return Token (END, -1);

                switch (c) {
                case ',': return Token (COMMA, -1);
                case ':': return Token (COLON, -1);
                case '(': return Token (LPAREN, -1);
                case ')': return Token (RPAREN, -1);
                case '%': return Token (VARIABLE, parse_int (0));
                case ' ':
                case '\t':
                case '\r':
                case '\n': continue;
                default:
                        if (is_numeric (c)) {
                                return Token (INTEGER, parse_int (c - '0'));
                        } else if (c == 'f' && match_str ("n")) {
                                return Token (FN, -1);
                        } else if (c == 'a' && match_str ("dd")) {
                                return Token (INSTRUCTION_ADD, -1);
                        } else if (c == 's' && match_str ("ub")) {
                                return Token (INSTRUCTION_SUB, -1);
                        } else if (c == 'm' && match_str ("ul")) {
                                return Token (INSTRUCTION_MUL, -1);
                        } else if (c == 'd' && match_str ("iv")) {
                                return Token (INSTRUCTION_DIV, -1);
                        } else if (c == 's' && match_str ("tore")) {
                                return Token (INSTRUCTION_STORE, -1);
                        } else if (c == 'j' && match_str("ump")) {
                                return Token(INSTRUCTION_JUMP, -1);
                        } else if (c == 'j' && match_str("umpif")) {
                                return Token(INSTRUCTION_JUMPIF, -1);
                        } else {
                                struct Token str = Token (STR, 0);

                                parse_str (c, str.fn_name);

                                return str;
                        }
                }
        }
}

struct Token peek_token ()
{
        return curr_token;
}

struct Token advance_token ()
{
        struct Token old_token = curr_token;

        if (TOKEN_TYPE (old_token) != END)
                curr_token = next_token ();

        return old_token;
}

bool match_token (enum TokenType t)
{
        if (TOKEN_TYPE (curr_token) == t) {
                advance_token ();
                return true;
        }

        return false;
}

struct Token consume_token (enum TokenType t, char *error_message, ...)
{
        struct Token curr = peek_token ();
        if (TOKEN_TYPE (curr) == t) {
                advance_token ();
                return curr;
        }

        va_list args;
        va_start (args, error_message);
        vfprintf (stderr, error_message, args);
        va_end (args);
        exit (EXIT_FAILURE);
}

void threeaddr_init_parser (char *ir)
{
        ir_source = ir;
        ir_source_index = 0;
        curr_token = next_token();
}
