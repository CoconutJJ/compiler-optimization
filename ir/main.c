#include "constant_prop.h"
#include "deadcode_elimination.h"
#include "function.h"
#include "ir_gen.h"
#include "mem.h"
#include "parser.h"
#include "ssa.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage ()
{
        printf ("usage: ./ir --ir_file|-f IR_FILE --pass|-p PASSES \n"
                "Flag Arguments:\n"
                "       IR_FILE: path to IR file\n"
                "       PASSES: Comma seperated list of passes\n"
                "               ssa - SSA tranformation\n"
                "               dce - Deadcode elimination\n"
                "               sscp - Sparse Simple Constant Propagation\n"
                "               cfg - Output Control Flow Graph ASCII art\n"
                "               disp - Output IR\n");
}

int main (int argc, char **argv)
{
        struct option long_opts[] = {
                (struct option){ .name = "ir_file", .has_arg = 1, .val = 'f', .flag = NULL },
                (struct option){ .name = "pass", .has_arg = 1, .val = 'p', .flag = NULL },
                (struct option){ 0 }
        };

        char *ir_file_name = NULL, *passes = NULL;
        int opt_index = 0, c;

        while ((c = getopt_long (argc, argv, "f:p:h", long_opts, &opt_index)) != EOF) {
                switch (c) {
                case 'f': ir_file_name = optarg; break;
                case 'p': passes = optarg; break;
                case 'h':
                        print_usage ();
                        exit (EXIT_SUCCESS);
                        break;
                case '?': exit (EXIT_FAILURE); break;
                default: fprintf (stderr, "error: unknown argument %c", c); exit (EXIT_FAILURE);
                }
        }

        if (!ir_file_name) {
                print_usage ();
                exit (EXIT_FAILURE);
        }

        FILE *ir_fp = fopen (ir_file_name, "r");

        if (!ir_fp) {
                perror ("fopen");
                exit (EXIT_FAILURE);
        }

        fseek (ir_fp, 0, SEEK_END);
        size_t ir_file_size = ftell (ir_fp);
        fseek (ir_fp, 0, SEEK_SET);

        char *ir_source = ir_malloc (ir_file_size + 1);

        if (fread (ir_source, ir_file_size, 1, ir_fp) < 1) {
                fprintf (stderr, "error: Unexpected end of file");
                exit (EXIT_FAILURE);
        }

        ir_source[ir_file_size] = '\0';

        struct Function *function = ParseIR (ir_source);

        char *pass = strtok (passes, ",");

        if (!pass) {
                fprintf (stderr, "error: No passes specified. Use argument -p pass1,pass2,... to specify.\n");
                exit (EXIT_FAILURE);
        }

        while (pass) {
                if (strcmp (pass, "ssa") == 0) {
                        SSATranslation (function);
                } else if (strcmp (pass, "dce") == 0) {
                        if (!function->is_ssa_form) {
                                fprintf (
                                        stderr,
                                        "error: Deadcode Elimination pass (dce) requires IR to be in SSA form first. Ensure `ssa` is in your pass list and before `dce`");
                                exit (EXIT_FAILURE);
                        }

                        RemoveDeadCode (function);
                } else if (strcmp (pass, "sscp") == 0) {
                        if (!function->is_ssa_form) {
                                fprintf (
                                        stderr,
                                        "error: Sparse Simple Constant Propagation pass (sscp) requires IR to be in SSA form first. Ensure `ssa` is in your pass list and before `sscp`");
                                exit (EXIT_FAILURE);
                        }

                        SSCP (function);
                } else if (strcmp (pass, "cfg") == 0) {
                        PrintFunction (function);
                } else if (strcmp (pass, "disp") == 0) {
                        EmitInit ();
                        EmitFunction (function);
                }

                pass = strtok (NULL, ",");
        }

        ir_free_all ();
}
