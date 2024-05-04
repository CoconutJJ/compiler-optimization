#pragma once
#include <stddef.h>
#define container_of(list_ptr, type, member) (type*)((char*)(list_ptr) - (char*)offsetof(type, member))


struct list_head {
  struct list_head *next;
  struct list_head *prev;
};

void list_entry_init (struct list_head *head);
struct list_head *list_next (struct list_head *head);
struct list_head *list_prev (struct list_head *head);
/**
        Inserts `new` right after head. If there was an element originally
        after head, `new` is modifed to point to it
**/
void list_insert_after (struct list_head *head, struct list_head *new);
/**
        Inserts `new` right before head. If there was an element originally before head,
        this one is modified to point to new instead.
**/
struct list_head *list_insert_before (struct list_head *head, struct list_head *new);
struct list_head *list_end (struct list_head *head);
