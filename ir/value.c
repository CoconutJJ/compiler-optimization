#include "global_constants.h"
#include "array.h"
#include "value.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
static size_t CURRENT_VALUE_NO = 0;

void Value_init (struct Value *value)
{
        assert (CURRENT_VALUE_NO < VALUE_TABLE_SIZE);

        value->value_no = CURRENT_VALUE_NO++;
        value->uses = NULL;
        value->uses_count = 0;
        value->uses_size = 0;
        DYNARR_INIT (value->uses, value->uses_count, value->uses_size, sizeof (struct Use));
}

void Use_init (struct Use *use)
{
        use->operand_no = -1;
        use->usee = NULL;
        use->user = NULL;
}


struct Use *Value_create_use (struct Value *value)
{
        struct Use *new_use = DYNARR_ALLOC (value->uses, value->uses_count, value->uses_size, sizeof (struct Use));

        Use_init (new_use);

        new_use->usee = value;

        return new_use;
}
