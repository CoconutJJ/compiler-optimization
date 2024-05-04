#include "linked_list.h"

void list_entry_init (struct list_head *head)
{
        head->next = NULL;
        head->prev = NULL;
}

/**
        Inserts `new` right after head. If there was an element originally
        after head, `new` is modifed to point to it
**/
void list_insert_after (struct list_head *head, struct list_head *new)
{
        struct list_head *old_next = head->next;
        head->next = new;
        new->prev = head;
        new->next = old_next;
}

/**
        Inserts `new` right before head. If there was an element originally before head,
        this one is modified to point to new instead.
**/
struct list_head *list_insert_before (struct list_head *head, struct list_head *new)
{
        struct list_head *old_before = head->prev;

        new->next = head;
        head->prev = new;

        if (!old_before)
                return new;

        old_before->next = new;
        new->prev = old_before;
        return head;
}

struct list_head *list_end (struct list_head *head)
{
        while (head->next)
                head = head->next;

        return head;
}

struct list_head *list_next (struct list_head *head)
{
        return head->next;
}

struct list_head *list_prev (struct list_head *head)
{
        return head->prev;
}
