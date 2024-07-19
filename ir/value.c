#include "value.h"
#include "array.h"
#include "global_constants.h"
#include "instruction.h"
#include "lexer.h"
#include "mem.h"
#include "utils.h"
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

        struct Array target_uses = Array_copy (&target->uses);

        while ((use = Array_iter (&target_uses, &iter_count)) != NULL) {
                struct Instruction *inst = AS_INST (use->user);
                InstructionSetOperand (inst, replacement, use->operand_no);
        }
        Array_free (&target_uses);
        ASSERT (Value_Use_count (target) == 0, "Value use list is corrupted!");
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

void Use_destroy (struct Use *use)
{
        ir_free (use);
}

void Use_link (struct Value *user, struct Value *usee, int usee_operand_no)
{
        if (VALUE_IS_CONST (usee))
                return;

        struct Use *use = ir_malloc (sizeof (struct Use));

        use->operand_no = usee_operand_no;
        use->usee = usee;
        use->user = user;

        Array_push (&usee->uses, use);
}

bool Use_unlink (struct Value *user, struct Value *usee, int usee_operand_no)
{
        if (VALUE_IS_CONST (usee))
                return true;

        struct Use *curr_use;
        size_t iter_count = 0;

        size_t index = 0;
        while ((curr_use = Array_iter (&usee->uses, &iter_count)) != NULL) {
                if (curr_use->user == user && curr_use->operand_no == usee_operand_no) {
                        Use_destroy (curr_use);
                        Array_delete (&usee->uses, index);
                        return true;
                }
                index++;
        }

        fprintf (stderr, "No use found!\n");

        return false;
}
