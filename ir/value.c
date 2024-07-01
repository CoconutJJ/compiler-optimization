#include "value.h"
#include "array.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "mem.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
static size_t CURRENT_VALUE_NO = 0;

void Value_init (struct Value *value)
{
        value->value_no = CURRENT_VALUE_NO++;
        value->token = Token (NIL, -1);

        Array_init (&value->uses);
}

void Value_free (struct Value *value)
{
        Array_free (&value->uses);
}

void Value_set_token (struct Value *value, struct Token token)
{
        value->token = token;
}

struct Value *Value_Use_iter (struct Value *value, size_t *iter_count)
{
        struct Use *use = Array_iter (&value->uses, iter_count);

        if (!use) {
                return NULL;
        }

        return use->user;
}

void Value_Replace_All_Uses_With (struct Value *target, struct Value *replacement)
{
        struct Use *use;
        size_t iter_count = 0;
        while ((use = Array_iter (&target->uses, &iter_count)) != NULL) {
                struct Instruction *inst = AS_INST (use->user);
                Instruction_set_operand (inst, replacement, use->operand_no);
        }
        // free all uses of the target and clear out use array
        Array_apply (&target->uses, (ArrayApplyFn)Use_free);
        Array_empty (&target->uses);
}

size_t Value_Use_count (struct Value *value)
{
        return Array_length (&value->uses);
}

void Use_init (struct Use *use)
{
        use->operand_no = -1;
        use->usee = NULL;
        use->user = NULL;
}

void Use_free (struct Use *use)
{
        ir_free (use);
}

void Use_link (struct Value *user, struct Value *usee, int usee_operand_no)
{
        struct Use *use = ir_malloc (sizeof (struct Use));

        use->operand_no = usee_operand_no;
        use->usee = usee;
        use->user = user;

        Array_push (&usee->uses, use);
}
