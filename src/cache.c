#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "cache.h"
#include "lru.h"

// Hash table implementation adapted from https://gist.github.com/tonious/1377667

struct entry_s
{
    key_type *key;
    val_type *value;
    struct entry_s *next;
    struct entry_s *prev;
    uint64_t size; // Size of entry, used for computing size
};
    
typedef struct entry_s entry_t;
struct cache_obj
{
    uint32_t size;
    struct entry_s **table;
    hash_func hash; // Has function, which can be user specified
    lru_t lru; // Object for storing LRU meta data
};

// Hash function
/**
 * Hash function based on Jenkin's one at a time hash
 * See https://en.wikipedia.org/wiki/Jenkins_hash_function
 */
uint64_t default_hash(key_type *key ) {

    char *tmp_key = NULL;
    tmp_key = (char *)key;
    uint64_t hashval = 0;
    uint32_t i = 0;

    /* Convert our string to an integer */
    while( i < strlen(tmp_key) && hashval < UINT64_MAX ) {
        hashval += tmp_key[i];
        hashval += hashval << 10;
        hashval ^= hashval >> 6;
        i++;
    }
    hashval += hashval << 3;
    hashval ^= hashval >> 11;
    hashval += hashval << 15;

    printf("Hashed the key %" PRIu8 " to the hashed value %" PRIu64 ".\n",
           *(uint8_t*)key, hashval);
    
    return hashval;
}

uint64_t hash_to_location(hash_func hash, key_type key, uint32_t cache_size)
{
    return hash(key) % cache_size;
}

bool test_val(cache_t cache, uint32_t pos, key_type key)
{
    entry_t *entry = cache->table[pos];
    if (entry == NULL) {
        return false;
    }
    else {
        printf("Our key: %" PRIu8 "\n", *key);
        printf("Entry key: %" PRIu8 "\n", *(uint8_t*)(entry->key));
        return strcmp((const char*)entry->key, (const char*) key) == 0;
    }
}

cache_t create_cache(uint64_t maxmem, hash_func hash)
{
    struct cache_obj* cache = malloc(sizeof(struct cache_obj));
    assert(cache != NULL);

    uint32_t size = maxmem / sizeof(entry_t);
    printf("Size of cache: %" PRIu32 "\n", size);

    cache->table = malloc( sizeof(entry_t *) * size );
    assert(cache->table != NULL);

    for (uint32_t i = 0; i < size; i++)
        cache->table[i] = NULL;

    assert(cache->table[4] == NULL);
    
    cache->size = size;

    // Add hash function
    cache->hash = (hash != NULL) ? hash : default_hash;

    // Create LRU object
    cache->lru = lru_create();
    
    return cache;
}

entry_t *create_entry(key_type key, val_type val, uint32_t val_size)
{
    entry_t *new_entry;

    new_entry = malloc(sizeof(entry_t));
    assert(new_entry != NULL);

    new_entry->key = malloc(sizeof(val_type) +1);
    memcpy(new_entry->key, key, sizeof(val_type));

    // Create entry for value
    val_type *value = malloc(val_size);
    assert(value != NULL);
    memcpy(value, val, val_size);
    
    new_entry->value = value;
    new_entry->size = val_size;
    new_entry->next = NULL;
    new_entry->prev = NULL;
    
    return new_entry;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size)
{
    uint32_t location = hash_to_location(cache->hash, key, cache->size);
    printf("Looking at location %" PRIu32 "\n", location);
    entry_t *prev = NULL;
    entry_t *next = NULL;
    entry_t *new_node = NULL;

    // Bump the value in the LRU
    lru_bump(cache->lru, (uint8_t*)key);
    
    next = cache->table[location];
    
    while(next != NULL && next->key != NULL &&
          strcmp((const char*)key, (const char*)next->key) != 0) {
        prev = next;
        next = next->next;
    }

    if (next == NULL) {
        // If it's not already in the cache, add it!
        new_node = create_entry(key, val, val_size);
        if (prev != NULL && next == NULL) { // At the end
            new_node->prev = prev;
            prev->next = new_node;
        }
        else {
            cache->table[location] = new_node;
        }
        printf("Added cache entry at location %" PRIi32 " with size %" PRIu64 " \n", location, new_node->size);
    }
    else {
        // If it is in the cache, update the value
        // Create entry for value
        free(next->value);
        next->value = (val_type *)strdup(val);
        next->size = val_size;
    }
}

entry_t *get_entry(cache_t cache, key_type key)
{
    uint32_t location = hash_to_location(cache->hash, key, cache->size);
    printf("Getting value at location: %" PRIu32 "\n", location);
    entry_t *node = NULL;
    node = cache->table[location];
    while(node != NULL && node->key != NULL &&
          strcmp((const char*)node->key, (const char*)key) != 0) {
        node = node->next;
    }    
    if (node == NULL) return NULL;
    else return node;
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    entry_t *entry = get_entry(cache, key);

    // Bump entry in cache
    lru_bump(cache->lru, (uint8_t*)key);
    
    if (entry == NULL) { // Miss
        printf("Miss!\n");
        *val_size = 0;
        return NULL; 
    }
    else { // Hit
        printf("Hit!\n");
        *val_size = entry->size;
        return entry->value;
    }
}

void cache_delete(cache_t cache, key_type key)
{
    uint32_t location = hash_to_location(cache->hash, key, cache->size);
    entry_t *prev = NULL;
    entry_t *current = NULL;
    entry_t *next = NULL;

    // Remove entry in LRU
    lru_remove(cache->lru, (uint8_t*)key);
    
    current = cache->table[location];
    
    while(current != NULL && current->key != NULL &&
          strcmp((const char*)key, (const char*)current->key) != 0) {
        prev = current;
        current = current->next;
    }
    
    if (current == NULL) {
        printf("Could not delete entry: Entry not found.\n");
        return;
    }

    // Restitch linked list back together
    next = current->next;
    if (next == NULL && prev != NULL) {
        // If current was at end of linked list
        prev->next = NULL;
    }
    else if (next != NULL && prev != NULL) {
        // If current was in middle of linked list
        prev->next = next;
        next->prev = prev;
    }
    else {
        // If current was at beginning of linked list
        cache->table[location] = next;
    }

    // Free the old entry
    free(current->value);
    free(current->key);
    free(current);
}

uint64_t cache_space_used(cache_t cache)
{
    uint32_t size = 0;
    // Iterate through table entries
    for (uint32_t i = 0; i < cache->size; i++) {
        entry_t *current_node = cache->table[i];
        // Iterate through linked list
        while (current_node != NULL) 
            size += current_node->size;
    }
    return size;
}

void destroy_cache(cache_t cache)
{
    // Iterate through table entries
    for (uint32_t i = 0; i < cache->size; i++) {
        entry_t *current = cache->table[i],
            *prev = cache->table[i];
        // Go to end of linked list
        while (current != NULL) {
            prev = current;
            current = current->next;
        }
        // Free items off of the linked list
        while (prev != NULL) {
            free(current);
            current = prev;
            prev = prev->prev;
        }
    }
    free(cache->table);

    lru_destroy(cache->lru);
    
    free(cache);
}

