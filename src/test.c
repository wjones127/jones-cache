#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "testing.h"
#include "cache.h"

void test_basics()
{
    cache_t cache = create_cache(65536, NULL);
    uint32_t val_size = 0;

    const uint8_t
        *key1 = (uint8_t*)"One key",
        *key2 = (uint8_t*)"Two key",
        *key3 = (uint8_t*)"Red key";

    uint8_t *value1 = (uint8_t*) "First value";
    uint64_t value2 = 20039;
    int32_t value3 = -12;

    cache_set(cache, key1, value1, strlen((char *)value1) + 1);
    cache_set(cache, key2, &value2, sizeof(value2));
    cache_set(cache, key3, &value3, sizeof(value3));

    uint8_t *result1 = (uint8_t *)cache_get(cache, key1, &val_size);
    uint64_t *result2 = (uint64_t *)cache_get(cache, key2, &val_size);
    int32_t *result3 = (int32_t *)cache_get(cache, key3, &val_size);

    bool results_not_null = result1 != NULL && result2 != NULL && result3 != NULL;
    bool string_result_correct = strcmp((const char*)value1, (const char*)result1) == 0;
    bool uint64_result_correct = *result2 == value2;
    bool int32_result_correct = *result3 == value3;
        
    test(results_not_null, "Get returns non null pointers when retrieving valid entries.");
    test(string_result_correct, "Cache can store and retrieve strings");
    test(uint64_result_correct, "Cache can store and retrieve uint64_t integers correctly.");
    test(int32_result_correct, "Cache can store and retrieve int32_t integers correctly.");
    test(val_size == sizeof(value3), "cache_get sets val_size pointer to value size.");

    destroy_cache(cache);
}

void test_get_modified()
{
    cache_t cache = create_cache(65536, NULL);
    uint32_t val_size = 0;

    const uint8_t *key = (uint8_t *)"weather";
    uint8_t *value = (uint8_t *)"sunny";

    cache_set(cache, key, value, strlen((char *)value) + 1);

    value = (uint8_t *)"pouring rain forever";
    cache_set(cache, key, value, strlen((char *)value) + 1);

    uint8_t *result = (uint8_t *)cache_get(cache, key, &val_size);

    bool value_is_updated = result != NULL && strcmp((const char*)value, (const char*)result) == 0;
    bool val_size_updated = val_size == strlen((char *)value) + 1;

    test(value_is_updated, "Setting to same key updates the value.");
    test(val_size_updated, "Value size from get is updated by cache_set.");
    
    destroy_cache(cache);
}

void test_deletion()
{

}

void test_get_nonexistant()
{
    cache_t cache = create_cache(65536, NULL);
    uint32_t val_size = 1;

    const uint8_t *key = (uint8_t *)"weather";

    uint8_t *result = (uint8_t *)cache_get(cache, key, &val_size);

    bool get_nothing_gives_null = result == NULL;
    bool nonexistant_val_size_is_zero = val_size == 0;

    test(get_nothing_gives_null, "Get return NULL when key is missing.");
    test(nonexistant_val_size_is_zero, "Get sets value size to zero when key is missing.");

    destroy_cache(cache);
}

void test_too_big()
{
    cache_t cache = create_cache(10, NULL);
    uint32_t val_size = 0;

    const uint8_t *key = (uint8_t *)"chrome";
    uint8_t *value = (uint8_t *)"This is supposed to be too big to fit into this cache";

    cache_set(cache, key, value, strlen((char *)value) + 1);
    uint8_t *result = (uint8_t *)cache_get(cache, key, &val_size);

    bool large_val_wasnt_saved = result == NULL;

    test(large_val_wasnt_saved, "Cache does not store large values.");

    destroy_cache(cache);
}

void test_copying_keys()
{
    cache_t cache = create_cache(65536, NULL);
    uint32_t val_size = 0;

    uint8_t *key1 = (uint8_t *)"original";
    uint8_t *key2 = (uint8_t *)"original";
    uint8_t *value = (uint8_t *)"nothing seemed to turn out right";

    cache_set(cache, key1, value, strlen((char *)value) + 1);

    key1 = (uint8_t *)"fake";

    uint8_t *result = (uint8_t *)cache_get(cache, key2, &val_size);

    bool key_still_valid = result != NULL;

    test(key_still_valid, "Changing original key does not change key in cache");

    destroy_cache(cache);
}

void test_copying_vals()
{
    
}

void test_lru()
{

}


int main(int argc, char *argv[])
{
    test_basics();
    test_get_modified();
    test_deletion();
    test_get_nonexistant();
    test_too_big();
    test_copying_keys();
    test_copying_vals();
    test_lru();
}
