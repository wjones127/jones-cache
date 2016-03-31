#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "testing.c"
#include "lru.h"

int main(int arg, char *argv[])
{
    lru_t lru = lru_create();

    // Can create an entry and bump it
    uint8_t *key1 = (uint8_t*) "First key";
    lru_bump(lru, key1);
    uint8_t *result = NULL;
    result = lru_get(lru);
    test(strcmp((const char*)result, (const char*)key1) == 0,
         "Can add one key and the get it");

    // If queue is empty, return NULL
    result = lru_get(lru);
    test(result == NULL, "If queue is empty, get returns NULL.");

    // Items can be removed
    lru_bump(lru, key1);
    lru_remove(lru, key1);
    result = lru_get(lru);
    test(result == NULL, "If an item is added and then removed, the queue is still empty");

    // Removing from an empty LRU does not give an error
    lru_remove(lru, key1);
    
    // Items can be bumped
    uint8_t *key2 = (uint8_t*) "Second Key";
    lru_bump(lru, key1);
    lru_bump(lru, key2);
    lru_bump(lru, key1);
    result = lru_get(lru);
    test(result != NULL, "Bumping doesn't fuck up values");
    test(strcmp((const char*)result, (const char*)key2) == 0,
          "Two items added; first added bumped; check lru_get gives the second one.");
    result = lru_get(lru);
    test(result != NULL && strcmp((const char*)result, (const char*)key1) == 0,
         "LRU can store multiple values");

    // Multiple items can be stored just fine
    uint8_t *key3 = (uint8_t*) "Third key";
    lru_bump(lru, key1);
    lru_bump(lru, key2);
    lru_bump(lru, key3);
    lru_bump(lru, key2);
    uint8_t *result1 = NULL;
    uint8_t *result2 = NULL;
    uint8_t *result3 = NULL;
    uint8_t *result4 = NULL;
    result1 = lru_get(lru);
    result2 = lru_get(lru);
    result3 = lru_get(lru);
    result4 = lru_get(lru);
    test(strcmp((const char*)result1, (const char*)key1) == 0 &&
         strcmp((const char*)result2, (const char*)key3) == 0 &&
         strcmp((const char*)result3, (const char*)key2) == 0 &&
         result4 == NULL,
         "Add 1, 2, 3; bump 2; get 4 times; check output is 1, 3, 2, NULL");
    
    lru_destroy(lru);
}
