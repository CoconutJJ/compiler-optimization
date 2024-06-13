#include "global_constants.h"
#include "threeaddr_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUFFER_SIZE (5 * (MAX_IDENTIFIER_LEN + 1))

static char token_buffer[TOKEN_BUFFER_SIZE];
static size_t token_buffer_count = 0;

char *token_buffer_alloc_str (size_t len)
{
        if (token_buffer_count + len > TOKEN_BUFFER_SIZE) {
                token_buffer_count = 0;
        }

        char *start = &token_buffer[token_buffer_count];

        token_buffer_count += len;

        return start;
}

char *Token_to_str (struct Token t)
{
        switch (TOKEN_TYPE (t)) {
        case STR:
        case VARIABLE: {
                char *buf = token_buffer_alloc_str (strlen (t.str_value) + 1);
                strcpy (buf, t.str_value);

                return buf;
        };
        case INSTRUCTION_CMP: return "cmp";
        case INSTRUCTION_ALLOCA: return "alloca";
        case INSTRUCTION_ADD: return "add";
        case INSTRUCTION_SUB: return "sub";
        case INSTRUCTION_MUL: return "mul";
        case INSTRUCTION_DIV: return "div";
        case INSTRUCTION_STORE: return "store";
        case INSTRUCTION_LOAD: return "load";
        case INSTRUCTION_JUMP: return "jump";
        case INSTRUCTION_JUMPIF: return "jumpif"; break;
        case INSTRUCTION_PHI: return "phi";
        case COMMA: return ",";
        case FN: return "fn";
        case LPAREN: return "(";
        case RPAREN: return ")";
        case INTEGER: {
                int len = snprintf (NULL, 0, "%d", t.value);

                char *buf = token_buffer_alloc_str (len + 1);

                snprintf (buf, len, "%d", t.value);

                return buf;
        }
        case LABEL: {
                int len = snprintf (NULL, 0, "%d:", t.value);

                char *buf = token_buffer_alloc_str (len + 1);

                snprintf (buf, len, "%d:", t.value);

                return buf;
        }
        case COLON: return ":";
        case AT: return "@";
        case LCURLY: return "{";
        case RCURLY: return "}";
        case END: return "END";
        case NIL: return "NIL";
        }

        return NULL;
}
