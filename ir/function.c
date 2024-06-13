#include "function.h"
#include "array.h"
#include <assert.h>
void Argument_init (struct Argument *argument)
{
        argument->value.value_type = VALUE_ARGUMENT;
        Value_init (&argument->value);
}

void Function_init (struct Function *function)
{
        function->entry_basic_block = NULL;

        Array_init(&function->arguments, sizeof (struct Argument *));
}

void Function_add_argument(struct Function *function, struct Argument * argument) {
        
        argument->parent = function;

        Array_push(&function->arguments, &argument);

}
