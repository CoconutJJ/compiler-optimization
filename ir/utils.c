#include "global_constants.h"
#include "threeaddr_parser.h"

static char Token_buffer[MAX_IDENTIFIER_LEN + 1];

char *Token_to_str (struct Token t)
{
        switch (TOKEN_TYPE (t)) {
        case VARIABLE: {
                strcpy (Token_buffer, t.str_value);
                return Token_buffer;
        };
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
                sprintf (Token_buffer, "%d", t.value);
                return Token_buffer;
        }
        case LABEL: {
                sprintf (Token_buffer, "%d:", t.value);
                return Token_buffer;
        }
        case STR: {
                strcpy (Token_buffer, t.str_value);
                return Token_buffer;
        }
        case COLON: return ":";
        case AT: return "@";
        case LCURLY: return "{";
        case RCURLY: return "}";
        case END: return "END";
        }

        return NULL;
}

