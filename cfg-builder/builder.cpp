#include <_stdio.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <sys/errno.h>
#include <unistd.h>

enum TokenType { COLON, COMMA, NUMBER, NEWLINE, END };

struct Token {
        enum TokenType type;
        int value;
};

static char curr = '!';
static FILE *fp;
static struct Token curr_token;

char peek_char ()
{
        return curr;
}

int at_end ()
{
        return curr == '\0';
}
char advance_char ()
{
        if (curr == '\0')
                return curr;

        char old = curr;

        int new_char = fgetc (fp);

        if (new_char == EOF)
                curr = '\0';
        else
                curr = new_char;

        return old;
}

int match_char (char c)
{
        if (curr == c) {
                advance_char ();
                return 1;
        }

        return 0;
}

int parse_integer (int first_digit)
{
        while (1) {
                char c = peek_char ();

                if (!('0' <= c && c <= '9'))
                        return first_digit;

                first_digit *= 10;
                first_digit += c - '0';
                advance_char ();
        }
}

struct Token parse_token ()
{
        while (!at_end ()) {
                char c = advance_char ();
                switch (c) {
                case ':': return { .type = COLON, .value = -1 };
                case ',': return { .type = COMMA, .value = -1 };
                case '\n': return { .type = NEWLINE, .value = -1 };
                case ' ':
                case '\r':
                case '\t': continue;
                default:
                        if ('0' <= c && c <= '9')
                                return { .type = NUMBER, .value = parse_integer (c - '0') };

                        fprintf (stderr, "ERROR: unknown character %c", c);
                        exit (EXIT_FAILURE);
                }
        }

        return { .type = END, .value = -1 };
}

enum TokenType peek_token ()
{
        return curr_token.type;
}

struct Token advance_token ()
{
        if (curr_token.type == END)
                return curr_token;

        struct Token old_token = curr_token;
        curr_token = parse_token ();
        return old_token;
}

int match_token (enum TokenType type)
{
        if (peek_token () == type) {
                advance_token ();
                return 1;
        }

        return 0;
}

void graph_parser_init (char *graph_file)
{
        if (fp) {
                fclose (fp);
                fp = NULL;
        }

        fp = fopen (graph_file, "r");

        if (!fp) {
                perror ("fopen");
                exit (EXIT_FAILURE);
        }

        advance_char ();
        advance_token ();
}

void create_graph_new (char *graph_file, void (*make_child) (int parent, int child))
{
        graph_parser_init (graph_file);

        while (1) {
                struct Token key = advance_token ();

                if (match_token (END)) {
                        break;
                }

                if (key.type != NUMBER) {
                        fprintf (stderr, "ERROR: expected numeric node label\n");
                        exit (EXIT_FAILURE);
                }

                if (!match_token (COLON)) {
                        fprintf (stderr, "ERROR: expected ':' after node label\n");
                        exit (EXIT_FAILURE);
                }

                while (!at_end ()) {
                        struct Token child = advance_token ();

                        if (child.type != NUMBER) {
                                fprintf (stderr, "ERROR: expected numeric node label\n");
                                exit (EXIT_FAILURE);
                        }

                        make_child (key.value, child.value);

                        if (match_token (COMMA)) {
                                continue;
                        } else if (match_token (NEWLINE)) {
                                break;
                        } else {
                                fprintf (stderr, "ERROR: unexpected token.\n");
                                exit (EXIT_FAILURE);
                        }
                }
        }
}

