#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <assert.h>
#include "lru.h"

struct lru_obj {
    struct lru_node *first;
    struct lru_node *last;
};

struct lru_node {
    struct lru_node *next;
    uint8_t *key;
};


lru_t lru_create()
{
    struct lru_obj *lru = malloc(sizeof(struct lru_obj));
    assert(lru != NULL);

    lru->first = NULL;

    return lru;
}

void lru_destroy(lru_t lru)
{
    // Free all of the nodes in the linked list
    if (lru->first != NULL) {
        struct lru_node *current = lru->first;
        struct lru_node *next = lru->first;
        while (next !=  NULL) {
            next = current->next;
            free(current);
            current = next;
        }
    }

    free(lru);
}

void lru_remove(lru_t lru, uint8_t *key)
{
    // First, find the key to be removed
    struct lru_node *prev = NULL;
    struct lru_node *current = NULL;
    if (lru->first == NULL) return; // LRU is empty, so just exit
    current = lru->first;
    while (strcmp((const char*)current->key, (const char*)key) != 0) {
        prev = current;
        current = current->next;
    }
    // Now remove the key
    if (current == NULL) return; // The key is not present
    struct lru_node *next = NULL;
    next = current->next;
    free(current->key);
    free(current);
    if (prev != NULL) prev->next = next;
}

uint8_t *lru_get(lru_t lru)
{
    if (lru->first == NULL)
        return NULL;
    else {
        uint8_t *key = (uint8_t*)strdup((const char*)lru->first->key);
        lru_remove(lru, key);
        return key;
    }
}

struct lru_node *lru_create_node(uint8_t *key)
{
    struct lru_node *node = malloc(sizeof(struct lru_node));
    assert(node != NULL);

    node->next = NULL;
    node->key = (uint8_t*)strdup((const char*)key);

    return node;
}

void lru_bump(lru_t lru, uint8_t *key)
{
    // First, check if lru is empty
    if (lru->first == NULL) {
        struct lru_node *new_node = lru_create_node(key);
        lru->first = new_node;
        lru->last = new_node;
        return;
    }
    // If it's not empty, check if key is in linked list
    struct lru_node *node = lru->first;
    struct lru_node *prev = lru->first;
    while(strcmp((const char*)node->key, (const char*)key) != 0 &&
          node->next != NULL) {
        prev = node;
        node = node->next;
    }
    // If it is in the linked list, move the node to top of queue
    if (node == NULL) {
        prev->next = node->next;
        node->next = lru->first;
        lru->first = node;
    }
    // If it's not in the linked list create a new node
    else {
        struct lru_node *new_node = lru_create_node(key);
        new_node->next = lru->first;
        lru->first = new_node;
    }
}
