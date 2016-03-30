/**
 * The LRU eviction policy.
 * Maintains a queue of keys to evict. Can get last used entry or move an entry
 * bottom of queue. 
 */

#include<stdio.h>
#include<stdlib.h>

struct lru_obj;
typedef struct lru_obj *lru_t;

/**
 * Creates an lru object
 */
lru_t lru_create();

/**
 * Destroys an lru
 */
void lru_destroy(lru_t lru);

/**
 * Gets the first item in the queue, which is the last recently bumped item.
 */
uint8_t *lru_get(lru_t lru);

/**
 * Bumps the given key to the back of the queue.
 */
void lru_bump(lru_t lru, uint8_t *key);
