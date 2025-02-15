#include "lexer.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *ir_source = NULL;
static char *current_line = NULL;
static size_t ir_source_index = 0;
static size_t current_line_number = 1;
static size_t current_column_number = 1;
static struct Token curr_token;

struct Token Token (enum TokenType type, int value)
{
        return (struct Token){ .type = type,
                               .value = value,
                               .line = current_line,
                               .line_number = current_line_number,
                               .column_number = current_column_number };
}

static char peek_char ()
{
        return ir_source[ir_source_index];
}

static char advance_char ()
{
        char old = ir_source[ir_source_index];
        if (peek_char () != '\0') {
                ir_source_index++;
                current_column_number++;
        }
        return old;
}

static bool match_char (char c)
{
        if (peek_char () == c) {
                advance_char ();
                return true;
        }
        return false;
}

static bool match_str (char *s)
{
        if (strncmp (s, ir_source + ir_source_index, strlen (s)) == 0) {
                ir_source_index += strlen (s);
                current_column_number += strlen (s);
                return true;
        }

        return false;
}

static bool is_numeric (char c)
{
        return ('0' <= c && c <= '9');
}

static bool is_alpha (char c)
{
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static int parse_int (int lead_digit)
{
        char c;

        int value = lead_digit;
        while (is_numeric ((c = peek_char ()))) {
                value = 10 * value + (c - '0');
                advance_char ();
        }

        return value;
}

static void parse_str (char lead_char, char *buffer)
{
        buffer[0] = lead_char;

        int i = 1;

        char c;
        while (is_alpha (c = peek_char ()) || is_numeric (c)) {
                buffer[i++] = advance_char ();

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
                case '{': return Token (LCURLY, -1);
                case '}': return Token (RCURLY, -1);
                case ',': return Token (COMMA, -1);
                case ':': return Token (COLON, -1);
                case '(': return Token (LPAREN, -1);
                case ')': return Token (RPAREN, -1);
                case '%': return Token (VARIABLE, parse_int (0));
                case '[': return Token (LBRACKET, -1);
                case ']': return Token (RBRACKET, -1);
                case '#': {
                        while (peek_char () != '\n')
                                advance_char ();

                        continue;
                }
                case ' ':
                case '\t':
                case '\r': continue;
                case '\n': {
                        current_line_number++;
                        current_column_number = 1;
                        current_line = ir_source + ir_source_index;
                        continue;
                }
                default:

                        switch (c) {
                        case 'f':
                                if (match_char ('n'))
                                        return Token (FN, -1);
                                break;
                        case 'a':
                                if (match_str ("lloca"))
                                        return Token (INSTRUCTION_ALLOCA, -1);
                                else if (match_str ("dd"))
                                        return Token (INSTRUCTION_ADD, -1);
                                break;
                        case 'c':
                                if (match_str ("mp"))
                                        return Token (INSTRUCTION_CMP, -1);
                                break;
                        case 'l':
                                if (match_str ("oad"))
                                        return Token (INSTRUCTION_LOAD, -1);
                                else if (match_str ("abel"))
                                        return Token (LABEL_LITERAL, -1);

                                break;
                        case 's':
                                if (match_str ("ub"))
                                        return Token (INSTRUCTION_SUB, -1);
                                else if (match_str ("tore"))
                                        return Token (INSTRUCTION_STORE, -1);
                                break;
                        case 'm':
                                if (match_str ("ul"))
                                        return Token (INSTRUCTION_MUL, -1);
                                break;
                        case 'd':
                                if (match_str ("iv"))
                                        return Token (INSTRUCTION_DIV, -1);
                                break;
                        case 'j':
                                if (match_str ("umpif"))
                                        return Token (INSTRUCTION_JUMPIF, -1);
                                else if (match_str ("ump"))
                                        return Token (INSTRUCTION_JUMP, -1);
                                break;
                        case 'p':
                                if (match_str ("hi"))
                                        return Token (INSTRUCTION_PHI, -1);
                                break;
                        case 'r':
                                if (match_str ("et"))
                                        return Token (INSTRUCTION_RET, -1);
                                break;
                        default: break;
                        }

                        if (is_numeric (c)) {
                                int value = parse_int (c - '0');
                                return Token (match_char (':') ? LABEL : INTEGER, value);
                        } else {
                                struct Token str = Token (STR, 0);

                                parse_str (c, str.str_value);

                                return str;
                        }
                        break;
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

void va_error (struct Token target, char *title, char *annotation, va_list args)
{
        fprintf (stderr, "%s on line %ld:%ld\n\n", title, target.line_number, target.column_number);

        char *c = target.line;
        while (*c != '\n' && *c != '\0') {
                fputc (*c, stderr);
                c++;
        }
        fputc ('\n', stderr);
        size_t ncols = target.column_number;

        while (ncols-- > 1) {
                fputc (' ', stderr);
        }
        fprintf (stderr, "^_____");

        vfprintf (stderr, annotation, args);
        fputc ('\n', stderr);
}

void error (struct Token target, char *message, ...)
{
        va_list args;
        va_start (args, message);
        va_error (target, "error", message, args);
        va_end (args);
}

struct Token consume_token (enum TokenType t, char *error_message, ...)
{
        struct Token curr = peek_token ();

        if (match_token (t))
                return curr;

        va_list args;
        va_start (args, error_message);
        va_error (curr, "error", error_message, args);
        va_end (args);
        exit (EXIT_FAILURE);
}

void threeaddr_init_parser (char *ir)
{
        ir_source = ir;
        ir_source_index = 0;
        current_line = ir;
        curr_token = next_token ();
}
