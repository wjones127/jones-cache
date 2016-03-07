#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "cache.h"

struct cache_obj
{
    uint32_t *capacity;
    key_type *keys;
    uint32_t *locations;
    val_type *values;
};

/**
 * Finds the location in the cache of a given key.
 */
uint32_t cache_seek(cache_t cache, key_type key)
{
    uint32_t location = 0;
    while (cache->values[location] != key && location < *(cache->capacity))
        location++;
    return location;
}

cache_t create_cache(uint64_t maxmem)
{
    struct cache_obj* cache = malloc(sizeof(struct cache_obj));
    assert(cache != NULL);
    
    // capacity is max # of key-values pairs that can be stored within maxmem
    cache->capacity = (uint32_t*)(maxmem / (sizeof(key_type) + sizeof(val_type)));
    cache->keys = (key_type*)calloc(*(cache->capacity), sizeof(key_type));
    cache->locations = (uint32_t*)calloc(*(cache->capacity), sizeof(key_type));
    cache->values = (val_type*)calloc(*(cache->capacity), sizeof(val_type));
    
    return cache;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size)
{
    uint32_t location = cache_seek(cache, key);
    // If it's not already in the cache
    if (location != *(cache->capacity)) {
        printf("allocating");
    }
    // If is is already in the cache
    else {
        printf("overwriting");
    }
    
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    uint32_t location = cache_seek(cache, key);
    if (location != *(cache->capacity)) return cache->values[location]; // Hit
    else return NULL; // Miss
}

void cache_delete(cache_t cache, key_type key)
{
    uint32_t location = cache_seek(cache, key);
    if (location != *(cache->capacity)) {
        cache->keys[location] = 0;
        cache->values[location] = 0;
    }
}

uint64_t cache_space_used(cache_t cache)
{
    return sizeof(cache->values);
}

void destroy_cache(cache_t cache)
{
    free(cache->values);
    free(cache->keys);
    free(cache->capacity);
}

