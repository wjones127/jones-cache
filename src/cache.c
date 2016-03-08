#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include "cache.h"

// Hash table implementation adapted from https://gist.github.com/tonious/1377667

struct entry_s
{
    key_type *key;
    val_type *value;
    //    struct entry_s *next;
};
    
typedef struct entry_s entry_t;

struct cache_obj
{
    uint32_t size;
    struct entry_s **table;
};

bool test_val(cache_t cache, uint32_t pos, key_type key)
{
    if (cache->table[pos] == NULL) return false;
    else return cache->table[pos]->key == key;
}

/**
 * Finds the location in the cache of a given key.
 */
int32_t cache_seek(cache_t cache, key_type key)
{
    uint32_t key_index = 0;
    while (test_val(cache, key_index, key) == false && key_index < cache->size)
        key_index++;
    if (key_index == cache->size) return -1;
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

    cache->table = malloc( sizeof(entry_t) * size );

    for (uint32_t i = 0; i < size; i++)
        cache->table[i] = NULL;

    cache->size = size;
    
    return cache;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size)
{
    int32_t location = cache_seek(cache, key);

    if (location == -1) {
        // If it's not already in the cache, find a spot for it
        location = find_space(cache);
    }
    else {
        // If new to cache, allocate memory for values
        cache->table[location]->value = malloc(sizeof(val_type));
        cache->table[location]->key = malloc(sizeof(key_type));
    }
    cache->table[location]->value = val;
    cache->table[location]->key = key;
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    uint32_t location = cache_seek(cache, key);
    if (location != -1) return cache->table[location]->value; // Hit
    else return NULL; // Miss
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

