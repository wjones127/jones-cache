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
    
    lru_destroy(lru);
}
