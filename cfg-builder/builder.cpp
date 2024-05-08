#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <unistd.h>

void consume_whitespace (FILE *fp)
{
        int c;
        while ((c = fgetc (fp)) != EOF) {
                switch (c) {
                case ' ':
                case '\t': continue;
                default: ungetc (c, fp); return;
                }

                break;
        }
}

int parse_node_value (FILE *fp)
{
        consume_whitespace (fp);

        if (feof (fp))
                return -1;

        int c, numeral_value = 0;
        while ((c = fgetc (fp)) != EOF) {
                if ('0' <= c && c <= '9') {
                        int value = c - '0';
                        numeral_value *= 10;
                        numeral_value += value;
                        continue;
                }

                ungetc (c, fp);
                break;
        }

        return numeral_value;
}
int match_char (FILE *fp, char c)
{
        consume_whitespace (fp);

        char curr = fgetc (fp);
        if (curr == c) {
                return 1;
        }
        ungetc (curr, fp);
        return 0;
}

void create_graph (char *graph_file, void (*make_child) (int parent, int child))
{
        FILE *graph_file_fp = fopen (graph_file, "r");

        int c;
        enum parser_state { KEY, CHILD };
        enum parser_state state = KEY;
        int key_value = -1;

        while (!feof (graph_file_fp)) {
                switch (state) {
                case KEY: {
                        key_value = parse_node_value (graph_file_fp);

                        if (key_value < 0) {
                                return;
                        }

                        if (!match_char (graph_file_fp, ':')) {
                                int c = fgetc (graph_file_fp);
                                fprintf (stderr, "ERROR: expected ':' after node key, got %c instead\n", c);
                                exit (EXIT_FAILURE);
                        }
                        state = CHILD;
                        break;
                }
                case CHILD: {
                        int child_value = parse_node_value (graph_file_fp);

                        if (child_value < 0)
                                return;

                        make_child (key_value, child_value);

                        if (match_char (graph_file_fp, ',')) {
                                continue;
                        } else if (match_char (graph_file_fp, '\n')) {
                                state = KEY;
                                continue;
                        }

                        break;
                }
                }
        }
}
