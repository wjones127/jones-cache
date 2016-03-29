# jones-cache
A generic look-aside cache. This is part of an assignment for Topics in Computer Systems at Reed College.

## 1. Basic Cache operations

My first implementation was simply a cache with an array of structs, each with key and value data. Search here was, of course, slow because it had to linearly search the array for a key when looking something up. This is definitely not the kind of performance one wants in a cache. 

## 2. Testing

I created a small testing function in `testing.c`. My tests are pretty basic here, just making sure each of the functions are working.

## 3. Performance

To improve performance, we will use a hash table for the cache. We will take a hash function  as a parameter in the creation of the cache, though for convienience we provide a simple default. 

## 4. Collision Resolution

I made the cache use a doubly-linked list for each bin in the hash table to handle collisions. This creates a little bit more complex code, but it avoids the cost of using the hash function again or having to linearly probe. 

## 5. Dynamic Resizing

Unfortunately I didn't get to implementing resizing :weary:.

## 6. Eviction Policy

I implemented LRU as a simple linked list queue. The last recently used item is taken from the top of the queue and items can be bumped to the bottom of the queue. This uses O(n) space and getting is O(1), but it takes O(n) time to bump something to top of the queue. In hindsight, this isn't the greatest idea, because bumping—which needs to happen whenever we get from the cache—should be more common than getting—which happens whenever we need to evict. 
