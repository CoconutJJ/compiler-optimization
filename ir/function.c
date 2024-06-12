#include "function.h"
#include <assert.h>
void Argument_init (struct Argument *argument)
{
        argument->value.value_type = VALUE_ARGUMENT;
        Value_init (&argument->value);
}

void Function_init (struct Function *function)
{
        function->arguments_count = 0;
        function->entry_basic_block = NULL;
}

struct Argument *Function_create_argument (struct Function *function)
{
        assert (function->arguments_count < MAX_FN_ARG_COUNT);

        struct Argument *new_argument = function->arguments + function->arguments_count;

        Argument_init (new_argument);

        function->arguments_count++;

        return new_argument;
}
