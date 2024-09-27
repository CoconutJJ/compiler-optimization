#include "function.h"
#include "array.h"
#include "basicblock.h"
#include "dfa.h"
#include "map.h"

void ArgumentInit (struct Argument *argument)
{
        argument->value.value_type = VALUE_ARGUMENT;
        Value_init (&argument->value);
}

void FunctionInit (struct Function *function)
{
        function->entry_block = NULL;
        function->exit_block = NULL;
        function->is_ssa_form = false;
        Array_init (&function->arguments);
        hash_table_init (&function->block_number_map);
}

void FunctionComputeBlockNumberMapping (struct Function *function)
{
        if (hash_table_count (&function->block_number_map) > 0) {
                hash_table_free (&function->block_number_map);
                hash_table_init (&function->block_number_map);
        }

        struct Array traversal_order = reverse_postorder (function->entry_block);

        struct BasicBlock *block;
        size_t iter_count = 0;
        while ((block = Array_iter (&traversal_order, &iter_count)) != NULL) {
                hash_table_insert (&function->block_number_map, block->block_no, block);
        }

        Array_free (&traversal_order);
}

void FunctionAddArgument (struct Function *function, struct Argument *argument)
{
        argument->parent = function;

        Array_push (&function->arguments, argument);
}
