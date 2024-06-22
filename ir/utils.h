#pragma once

#define UNREACHABLE(msg)                                                                                               \
        {                                                                                                              \
                fprintf (                                                                                              \
                        stderr, "Unreachable error in file %s, in function %s, on line %d: %s \n", __FILE__, __FUNCTION__, __LINE__, (msg)); \
                exit (EXIT_FAILURE);                                                                                   \
        }
char *Token_to_str (struct Token t);
