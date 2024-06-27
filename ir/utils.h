#pragma once
#include "lexer.h"

#define __FILENAME__ (strrchr (__FILE__, '/') ? strrchr (__FILE__, '/') + 1 : __FILE__)

#define UNREACHABLE(msg)                                                                                               \
        {                                                                                                              \
                fprintf (stderr,                                                                                       \
                         "Unreachable error in file %s, in function %s, on line %d: %s \n",                            \
                         __FILENAME__,                                                                                 \
                         __FUNCTION__,                                                                                 \
                         __LINE__,                                                                                     \
                         (msg));                                                                                       \
                exit (EXIT_FAILURE);                                                                                   \
        }

#define BUG(msg)                                                                                                       \
        {                                                                                                              \
                fprintf (stderr,                                                                                       \
                         "Bug in file %s, in function %s, on line %d: %s \n",                                          \
                         __FILENAME__,                                                                                 \
                         __FUNCTION__,                                                                                 \
                         __LINE__,                                                                                     \
                         (msg));                                                                                       \
                exit (EXIT_FAILURE);                                                                                   \
        }

#define ASSERT(cond, err_msg)                                                                                          \
        {                                                                                                              \
                if (!(cond)) {                                                                                         \
                        fprintf (stderr,                                                                               \
                                 "Assertion error in file %s, in function %s, on line %d: %s \n",                      \
                                 __FILENAME__,                                                                         \
                                 __FUNCTION__,                                                                         \
                                 __LINE__,                                                                             \
                                 (err_msg));                                                                           \
                        exit (EXIT_FAILURE);                                                                           \
                }                                                                                                      \
        }
char *Token_to_str (struct Token t);