#include "ir_parser.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
        if (argc < 2) {
                fprintf (stderr, "Expected IR file.\n");
                exit (EXIT_FAILURE);
        }
        struct option long_opts[] = {
                (struct option){ .name = "ir_file", .has_arg = 1, .val = 'f', .flag = NULL },
                 (struct option){ 0 }
        };
        
        char *ir_file_name = NULL;
        int opt_index = 0, c;

        while ((c = getopt_long (argc, argv, "f:", long_opts, &opt_index)) != EOF) {
                switch (c) {
                case 'f': ir_file_name = optarg; break;
                case '?': exit (EXIT_FAILURE); break;
                default: fprintf (stderr, "error: unknown argument %c", c); exit(EXIT_FAILURE);
                }
        }

        FILE *ir_fp = fopen (ir_file_name, "r");

        if (!ir_fp) {
                perror ("fopen");
                exit (EXIT_FAILURE);
        }

        fseek (ir_fp, 0, SEEK_END);
        size_t ir_file_size = ftell (ir_fp);
        fseek (ir_fp, 0, SEEK_SET);

        char *ir_source = malloc (ir_file_size + 1);
        fread (ir_source, ir_file_size, 1, ir_fp);
        ir_source[ir_file_size] = '\0';

        struct Function * function = parse_ir (ir_source);

        display_function(function);
}