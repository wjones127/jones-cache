#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "cache.h"

int main(int argc, char *argv[])
{
    cache_t cache = create_cache(65536, NULL);
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

    // TEST: Getting and retrieving several entries
    uint8_t *key4 = (uint8_t*)"One key";
    uint8_t *key5 = (uint8_t*)"Two key";
    uint8_t *key6 = (uint8_t*)"Red key";
    uint8_t *key7 = (uint8_t*)"Blue key";
    uint8_t *value4 = (uint8_t*) "First value";
    uint64_t value5 = 20039;
    int32_t value6 = -12;
    uint8_t *value7 = (uint8_t*) "final value";
    cache_set(cache, key4, value4, strlen((char *)value4) + 1);
    cache_set(cache, key5, &value5, sizeof(value5));
    cache_set(cache, key6, &value6, sizeof(value6));
    cache_set(cache, key7, value7, strlen((char *)value7) + 1);

    uint8_t *result4 = (uint8_t *)cache_get(cache, key4, &val_size);
    test(result4 != NULL && strcmp((const char*)value4, (const char*)result4) == 0,
         "Gets this entry");
    uint64_t *result5 = (uint64_t *)cache_get(cache, key5, &val_size);
    test(result5 != NULL && *result5 == value5,
         "Gets entry 5.");
    uint32_t *result6 = (uint32_t *)cache_get(cache, key6, &val_size);
    test(result6 != NULL && *result6 == value6,
         "Gets this entry");
    uint8_t *result7 = (uint8_t *)cache_get(cache, key7, &val_size);
    test(result7 != NULL && strcmp((const char*)value7, (const char*)result7) == 0,
         "Gets this entry");

    destroy_cache(cache);
}
