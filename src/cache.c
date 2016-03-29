#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "cache.h"

// Hash table implementation adapted from https://gist.github.com/tonious/1377667

struct entry_s
{
    key_type *key;
    val_type *value;
    struct entry_s *next;
    struct entry_s *prev;
    uint32_t size; // Size of entry, used for computing size
};
    
typedef struct entry_s entry_t;
struct cache_obj
{
    uint32_t size;
    struct entry_s **table;
};

// Hash function
uint32_t hash( cache_obj *cache, key_type *key ) {

    unsigned long int hashval;
    int i = 0;

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
        hashval += key[ i ];
        i++;
    }

    return hashval % cache->size;
}

bool test_val(cache_t cache, uint32_t pos, key_type key)
{
    entry_t *entry = cache->table[pos];
    if (entry == NULL) {
        return false;
    }
    else {
        printf("Our key: %" PRIu8 "\n", *key);
        printf("Entry key: %" PRIu8 "\n", *(entry->key));
        return strcmp(entry->key, (key_type) key) == 0;
    }
}

/**
 * Finds the location in the cache of a given key.
 */
int32_t cache_seek(cache_t cache, key_type key)
{
    uint32_t key_index = 0;

    while (test_val(cache, key_index, key) == false && key_index < cache->size -1)
        key_index++;
    
    if (key_index == cache->size - 1) return -1;
    else return key_index;
}
uint32_t find_space(cache_t cache)
{
    uint32_t pos = 0;
    while (cache->table[pos] != NULL) pos++;
    return pos;
}

cache_t create_cache(uint64_t maxmem, hash_func hash, uint8_t* add, uint8_t* remove)
{
    struct cache_obj* cache = malloc(sizeof(struct cache_obj));
    assert(cache != NULL);

    uint32_t size = maxmem / sizeof(entry_t);
    printf("Size of cache: %" PRIu32 "\n", size);

    cache->table = malloc( sizeof(entry_t *) * size );
    assert(cache->table != NULL);

    for (uint32_t i = 0; i < size; i++)
        cache->table[i] = NULL;

    cache->size = size;
    
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
    uint32_t location = hash(cache, key);
    entry_t prev = NULL;
    entry_t node = cache->table[location];
    while(strcmp(node->key, key) != 1  && node != NULL) {
        prev = node;
        node = node->next;
    }

    if (node = NULL) {
        // If it's not already in the cache, add it!
        entry_t new_node = create_entry(key, val, val_size);
        if (prev != NULL) {
            new_node->prev = prev;
            prev->next = new_node;
        }
        else {
            cache->table[location] = new_node;
        }
        printf("Added cache entry at location: %" PRIi32 "\n", location);
    }
    else {
        // If it is in the cache, update the value
        // Create entry for value
        memcpy(node->value, val, val_size);
        node->size = val_size;
    }
}

val_type get_value(cache_t cache, key_type key)
{
    int32_t location = cache_seek(cache, key);
    if (location == -1) return NULL;
    else return cache->table[location]->value;
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    val_type value = get_value(cache, key);
    //printf("found location: %" PRIi32 "\n", location);
    if (value == NULL) return NULL; // Miss
    else { // Hit
        *val_size = sizeof(*value);
        return value;
    }
}

void delete_entry(cache_t cache, uint32_t location) {
    if (cache->table[location] != NULL) {
        free(cache->table[location]->key);
        free(cache->table[location]->value);
        free(cache->table[location]);
        cache->table[location] = NULL;
    }
}

void cache_delete(cache_t cache, key_type key)
{
    int32_t location = cache_seek(cache, key);
    if (location != -1)
        delete_entry(cache, location);
}

uint64_t cache_space_used(cache_t cache)
{
    uint32_t size = 0;
    for (uint32_t i = 0; i < cache->size; i++)
        size += sizeof(cache->table[i]->value);
    return size;
}

void destroy_cache(cache_t cache)
{
    for (uint32_t i = 0; i < cache->size; i++)
        delete_entry(cache, i);
    free(cache->table);
    free(cache);
}

