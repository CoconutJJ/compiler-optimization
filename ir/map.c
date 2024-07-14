#include "map.h"
#include "global_constants.h"
#include "mem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Open addressing, linear probe hash table.

static unsigned long uint64_t_hash_function (uint64_t key)
{
        // Simple hash function for demonstration
        return key;
}

static void _hash_table_init (HashTable *table, size_t size)
{
        table->size = size;
        table->count = 0;
        table->buckets = ir_calloc (table->size, sizeof (HashTableEntry));
}
void hash_table_init (HashTable *table)
{
        _hash_table_init (table, MAP_INIT_SIZE_CNT);
}

// Create a new hash table
HashTable *hash_table_create (size_t size)
{
        HashTable *table = ir_malloc (sizeof (HashTable));

        _hash_table_init (table, size);

        return table;
}

void hash_table_empty (HashTable *table)
{
        memset (table->buckets, 0, table->count * sizeof (struct HashTableEntry *));
        table->count = 0;
}
static void _hash_table_insert (HashTable *table, uint64_t key, void *value)
{
        // Insert an item into the hash table
        uint64_t index = uint64_t_hash_function (key) % table->size;
        uint64_t size = table->size;
        while (size-- > 0) {
                struct HashTableEntry *entry = &table->buckets[index];

                if (!entry->isOccupied) {
                        entry->key = key;
                        entry->value = value;
                        entry->isOccupied = true;
                        entry->isDeleted = false;
                        table->count++;
                        return;
                } else if (entry->key == key) {
                        // replace entry if entry already exists
                        entry->value = value;
                        return;
                }

                index = (index + 1) % table->size;
        }
}
static void hash_table_resize_if_required (HashTable *table)
{
        HashTable resized_hash_table;

        if (table->count == table->size) {
                _hash_table_init (&resized_hash_table, table->size * 2);
        } else if (table->count < table->size / 4) {
                if (table->size <= MAP_INIT_SIZE_CNT)
                        return;

                _hash_table_init (&resized_hash_table, table->size / 2);
        } else {
                return;
        }

        uint64_t index = 0;
        while (index < table->size) {
                struct HashTableEntry *entry = &table->buckets[index];

                if (entry->isDeleted || !entry->isOccupied) {
                        index++;
                        continue;
                }
                _hash_table_insert (&resized_hash_table, entry->key, entry->value);

                index++;
        }

        hash_table_free (table);

        *table = resized_hash_table;
}

void hash_table_insert (HashTable *table, uint64_t key, void *value)
{
        hash_table_resize_if_required (table);

        _hash_table_insert (table, key, value);
}

// Search for an item in the hash table
static struct HashTableEntry *hash_table_find_entry (HashTable *table, uint64_t key)
{
        uint64_t index = uint64_t_hash_function (key) % table->size;
        uint64_t size = table->size;
        while (size-- > 0) {
                struct HashTableEntry *entry = &table->buckets[index];

                if (entry->isDeleted) {
                        index = (index + 1) % table->size;
                        continue;
                }

                if (!entry->isOccupied) {
                        return NULL;
                }

                if (entry->key == key) {
                        return entry;
                }

                index = (index + 1) % table->size;
        }

        return NULL;
}

void *hash_table_search (HashTable *table, uint64_t key)
{
        struct HashTableEntry *entry = hash_table_find_entry (table, key);

        if (!entry)
                return NULL;

        return entry->value;
}

void *hash_table_find_and_delete (HashTable *table, uint64_t key)
{
        struct HashTableEntry *entry = hash_table_find_entry (table, key);

        if (!entry)
                return NULL;

        void *value = entry->value;
        entry->isDeleted = true;
        entry->isOccupied = false;
        entry->value = NULL;
        table->count--;

        hash_table_resize_if_required (table);

        return value;
}

struct HashTableEntry *hash_table_entry_iter (HashTable *table, size_t *iter_count)
{
        while (*iter_count < table->size && !table->buckets[*iter_count].isOccupied)
                (*iter_count)++;

        if (*iter_count == table->size) {
                *iter_count = 0;
                return NULL;
        }

        struct HashTableEntry *entry = &table->buckets[*iter_count];

        (*iter_count)++;

        return entry;
}

size_t hash_table_count (HashTable *table)
{
        return table->count;
}

// Free the hash table
void hash_table_free (HashTable *table)
{
        ir_free (table->buckets);
}
