#include "parser.h"
#include "definition.h"
#include "variable.h"
#include <string.h>
char *ir_source = NULL;
size_t count = 0;

char advance_char ()
{
        if (ir_source[count] == '\0')
                return '\0';

        return ir_source[count++];
}

char peek_char ()
{
        return ir_source[count];
}

int match_char (char c)
{
        if (peek_char () == c) {
                advance_char ();
                return 1;
        }

        return 0;
}

int is_eof ()
{
        return ir_source[count] == '\0';
}

int is_alpha (char c)
{
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

int is_numeric (char c)
{
        return '0' <= c && c <= '9';
}
Token create_token (enum TokenType type)
{
        return (Token){
                .type = type,
                0,
        };
}

Token parse_identifier ()
{
        char identifier_name[MAX_IDENTIFIER_LEN];
        int n = 0;
        while (is_alpha (peek_char ()) || is_numeric (peek_char ())) {
                if (n == 20) {
                        fprintf (stderr, "error: identifier must be less than 10 characters.");
                        exit (EXIT_FAILURE);
                }

                identifier_name[n++] = advance_char ();
        }

        Token t;

        if (strcmp (identifier_name, "jmpif") == 0) {
                t = create_token (JUMPIF);
        } else if (strcmp (identifier_name, "jmp") == 0) {
                t = create_token (JUMP);
        } else if (strcmp (identifier_name, "phi") == 0) {
                t = create_token (PHI);
        } else {
                t = create_token (IDENTIFIER);
                strcpy(t.svalue, identifier_name);
        }

        return t;
}

Token parse_literal ()
{
        char numeral[MAX_LITERAL_LEN];
        int n = 0;
        while (is_numeric (peek_char ())) {
                if (n == 20) {
                        fprintf (stderr, "error: literal length cannot exceed %d characters", MAX_LITERAL_LEN);
                        exit (EXIT_FAILURE);
                }
                numeral[n++] = advance_char ();
                numeral[n] = '\0';
        }
        Token t = create_token (LITERAL);
        if (!match_char ('.')) {
                t.ivalue = strtol (numeral, NULL, 10);

                return t;
        }

        while (is_numeric (peek_char ())) {
                if (n == 20) {
                        fprintf (stderr, "error: literal length cannot exceed %d characters", MAX_LITERAL_LEN);
                        exit (EXIT_FAILURE);
                }
                numeral[n++] = advance_char ();
                numeral[n] = '\0';
        }

        t.dvalue = strtod (numeral, NULL);

        return t;
}

Token parse_token ()
{
        while (!is_eof ()) {
                switch (peek_char ()) {
                case '+': return create_token (ADD);
                case '-': return create_token (MINUS);
                case '*': return create_token (MULT);
                case '/': return create_token (DIV);
                case '=': return create_token (EQ);
                case ':': return create_token (COLON);
                case ',': return create_token (COMMA);
                case '(': return create_token (LEFT_PAREN);
                case ')': return create_token (RIGHT_PAREN);
                case ' ':
                case '\t': continue;
                default:

                        if (is_alpha (peek_char ())) {
                                return parse_identifier ();
                        } else if (is_numeric (peek_char ())) {
                                return parse_literal ();
                        } else {
                                fprintf (stderr, "error: unexpected symbol %c", peek_char ());
                                exit (EXIT_FAILURE);
                        }

                        break;
                }
        }

        return create_token (END_OF_FILE);
}
