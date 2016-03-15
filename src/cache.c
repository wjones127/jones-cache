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
    //    struct entry_s *next;
};
    
typedef struct entry_s entry_t;
/*struct cache_obj
{
    uint32_t size;
    struct entry_s **table;
};*/


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

cache_t create_cache(uint64_t maxmem)
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

    return new_entry;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size)
{
    int32_t location = cache_seek(cache, key);

    if (location == -1) {
        // If it's not already in the cache, find a spot and allocate memory
        location = find_space(cache);
        printf("Added cache entry at location: %" PRIi32 "\n", location);
    }

    entry_t *new_entry;
    new_entry = create_entry(key, val, val_size);
    cache->table[location] = new_entry;
    assert(cache->table[location] != NULL);
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    int32_t location = cache_seek(cache, key);
    printf("found location: %" PRIi32 "\n", location);
    if (location == -1) return NULL; // Miss
    else { // Hit
        val_type value = *(cache->table[location]->value);
        assert(*val_size == sizeof(value) && "Retrieved value has wrong size");
        return *(cache->table[location]->value);
    }
}

void delete_entry(cache_t cache, uint32_t location) {
    if (cache->table[location] != NULL) {
        free(cache->table[location]->key);
        free(cache->table[location]->value);
        cache->table[location] = NULL;
    }
}

void cache_delete(cache_t cache, key_type key)
{
    uint32_t location = cache_seek(cache, key);
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

