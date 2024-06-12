#pragma once

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
        struct HashTableEntry *next;
} HashTableEntry;

// Define a structure for the hash table
typedef struct HashTable {
        HashTableEntry **buckets;
        size_t size;
        size_t count;
} HashTable;

// Hash function for uint64_t keys
unsigned long uint64_t_hash_function (uint64_t key);

// Create a new hash table
HashTable *hash_table_create (size_t size);

// Create a new hash table entry
HashTableEntry *hash_table_entry_create (uint64_t key, void *value);
// Insert an item into the hash table
void hash_table_insert (HashTable *table, uint64_t key, void *value);

// Search for an item in the hash table
void *hash_table_search (HashTable *table, uint64_t key);
// Free the hash table
void hash_table_free (HashTable *table);

void hash_table_empty (HashTable *table);

void hash_table_init (HashTable *table, size_t size);