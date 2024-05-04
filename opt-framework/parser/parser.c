#include "parser.h"
#include "definition.h"

Variable *parse_variable ()
{
        Token variable = parse_token ();

        if (variable.type != IDENTIFIER || variable.type != LITERAL) {
                return NULL;
        }

        Variable *v = create_variable (variable.svalue);

        return v;
}

Definition *parse_definition ()
{
        Token dest = parse_token ();
}