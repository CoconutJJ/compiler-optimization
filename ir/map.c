#include "map.h"
#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Hash function for uint64_t keys
unsigned long uint64_t_hash_function (uint64_t key)
{
        // Simple hash function for demonstration
        return key;
}

void hash_table_init (HashTable *table, size_t size)
{
        table->size = size;
        table->count = 0;
        table->buckets = ir_calloc (table->size, sizeof (HashTableEntry *));
}
// Create a new hash table
HashTable *hash_table_create (size_t size)
{
        HashTable *table = ir_malloc (sizeof (HashTable));

        hash_table_init (table, size);

        return table;
}

// Create a new hash table entry
HashTableEntry *hash_table_entry_create (uint64_t key, void *value)
{
        HashTableEntry *entry = ir_malloc (sizeof (HashTableEntry));
        entry->key = key;
        entry->value = value;
        entry->next = NULL;
        return entry;
}

void hash_table_empty (HashTable *table)
{
        memset (table->buckets, 0, table->count * sizeof (struct HashTableEntry *));
        table->count = 0;
}

// Insert an item into the hash table
void hash_table_insert (HashTable *table, uint64_t key, void *value)
{
        unsigned long hash = uint64_t_hash_function (key);
        size_t index = hash % table->size;

        HashTableEntry *new_entry = hash_table_entry_create (key, value);
        HashTableEntry *current_entry = table->buckets[index];

        if (current_entry == NULL) {
                table->buckets[index] = new_entry;
        } else {
                while (current_entry->next != NULL) {
                        current_entry = current_entry->next;
                }
                current_entry->next = new_entry;
        }

        table->count++;
}

// Search for an item in the hash table
void *hash_table_search (HashTable *table, uint64_t key)
{
        unsigned long hash = uint64_t_hash_function (key);
        size_t index = hash % table->size;

        HashTableEntry *entry = table->buckets[index];
        while (entry != NULL) {
                if (entry->key == key) {
                        return entry->value;
                }
                entry = entry->next;
        }
        return NULL;
}

void *hash_table_find_and_delete (HashTable *table, uint64_t key)
{
        unsigned long hash = uint64_t_hash_function (key);
        size_t index = hash % table->size;

        HashTableEntry *entry = table->buckets[index];
        HashTableEntry *prev = NULL;

        while (entry != NULL) {
                if (entry->key == key) {
                        if (prev)
                                prev->next = entry->next;

                        void *value = entry->value;
                        free (entry);
                        return value;
                }
                prev = entry;
                entry = entry->next;
        }

        return NULL;
}

// Free the hash table
void hash_table_free (HashTable *table)
{
        for (size_t i = 0; i < table->size; i++) {
                HashTableEntry *entry = table->buckets[i];
                while (entry != NULL) {
                        HashTableEntry *next = entry->next;
                        free (entry);
                        entry = next;
                }
        }
        free (table->buckets);
        free (table);
}
