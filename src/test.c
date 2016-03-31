#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "testing.h"
#include "cache.h"

int main(int argc, char *argv[])
{
    cache_t cache = create_cache(256, NULL);
    uint32_t val_size = 0;
    
    // Create our first entry
    //uint8_t key1 = 29;
    uint8_t *key1 = (uint8_t *) "hello";
    uint64_t value1 = 256;
    cache_set(cache, key1, &value1, sizeof(value1));

    // TEST: cache can retrieve entry
    uint64_t *result = (uint64_t*)cache_get(cache, key1, &val_size);
    test(*result == 256, "Cache can retrieve an entry");
    test(val_size == sizeof(value1), "Cache sets val_size pointer on gets");

    // TEST: cache returns NULL for entry not added
    uint8_t *key2 = (uint8_t *) "world";
    result = (uint64_t*)cache_get(cache, key2, &val_size);
    test(result == NULL, "Cache returns NULL for key not in cache");
    test(val_size == 0, "Cache sets missing get key size to zero");

    // TEST: deleted entries don't show up
    cache_delete(cache, key1);
    result = (uint64_t*)cache_get(cache, key1, &val_size);
    test(result == NULL, "Deleted entries cannot be accessed");

    // TEST: Deleting nonexistant entry does not change the size
    uint64_t original_space_used = cache_space_used(cache);
    uint8_t *key3 = (uint8_t*) "meh";
    cache_delete(cache, key3);
    uint64_t later_space_used = cache_space_used(cache);
    test(original_space_used == later_space_used,
         "Deleting nonexistant entries does not changes space used.");

    // TEST: Getting updated items returns new value
    uint32_t value3 = 301;
    cache_set(cache, key1, &value3, sizeof(value3));
    uint32_t *result2 = (uint32_t*)cache_get(cache, key1, &val_size);
    test(*result2 == 301, "Cache returns newer value after an item is updated.");
    test(val_size == sizeof(value3),
         "Cache updates value size of updated values");


    destroy_cache(cache);
}
