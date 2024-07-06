#include "function.h"
#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "map.h"
#include <assert.h>
void Argument_init (struct Argument *argument)
{
        argument->value.value_type = VALUE_ARGUMENT;
        Value_init (&argument->value);
}

void Function_init (struct Function *function)
{
        function->entry_basic_block = NULL;

        Array_init (&function->arguments);
        hash_table_init (&function->block_number_map);
}

void FunctionComputeBlockNumberMapping (struct Function *function)
{
        if (hash_table_count (&function->block_number_map) > 0) {
                hash_table_free (&function->block_number_map);
                hash_table_init (&function->block_number_map);
        }

        struct Array traversal_order = reverse_postorder (function->entry_basic_block);

        struct BasicBlock *block;
        size_t iter_count = 0;
        while ((block = Array_iter (&traversal_order, &iter_count)) != NULL) {
                hash_table_insert (&function->block_number_map, block->block_no, block);
        }

        Array_free (&traversal_order);
}

void Function_add_argument (struct Function *function, struct Argument *argument)
{
        argument->parent = function;

        Array_push (&function->arguments, argument);
}
