#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "cache.h"

int main(int argc, char *argv[])
{
    assert(argc == 2 && "require one argument to run");
    uint64_t maxmem = strtol(argv[1], NULL, 0);

    cache_t cache = create_cache(maxmem);

    assert(cache->table[1] == NULL && "table entries should start as NULL");
    
    // We use pointers to refer to keys.
    key_type key;
    val_type value;
    uint32_t *key_size;

    // Create our first entry
    uint8_t key1val = 29;
    uint64_t value1 = 256;
    uint32_t key1_size = sizeof(value1);
    key = &key1val;
    value = &value1;
    key_size = &key1_size;
  
    cache_set(cache, key, value, *key_size);

    val_type result;
    result = cache_get(cache, key, key_size);
    //    assert(*result == 256);

    key1val = 40;
    result = cache_get(cache, key, key_size);
    //    assert(*result == NULL);
    

    destroy_cache(cache);
}
