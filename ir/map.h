#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the initial size of the hash table
#define INITIAL_SIZE 16

// Define a structure for a hash table entry
typedef struct HashTableEntry {
        uint64_t key;
        void *value;
        bool isDeleted;
        bool isOccupied;
} HashTableEntry;

// Define a structure for the hash table
typedef struct HashTable {
        HashTableEntry *buckets;
        size_t size;
        size_t count;
} HashTable;

void hash_table_init (HashTable *table);
HashTable *hash_table_create (size_t size);
void hash_table_empty (HashTable *table);
size_t hash_table_count (HashTable *table);
void hash_table_insert (HashTable *table, uint64_t key, void *value);

// Search for an item in the hash table
void *hash_table_search (HashTable *table, uint64_t key);
struct HashTableEntry *hash_table_entry_iter (HashTable *table, size_t *iter_count);
void *hash_table_find_and_delete (HashTable *table, uint64_t key);
// Free the hash table
void hash_table_free (HashTable *table);
