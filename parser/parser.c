#include "definition.h"

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

void parse_identifier ()
{
}

Definition *parse_set ()
{
}
