# jones-cache
A generic look-aside cache. This is part of an assignment for Topics in Computer Systems at Reed College.

## Instructions

To build and test the cache, use
```
make test_cache
```
To just build the cache
```
make build_cache
```
## Implementation Details
### 1. Basic Cache operations

My first implementation was simply a cache with an array of structs, each with key and value data. Search here was, of course, slow because it had to linearly search the array for a key when looking something up. This is definitely not the kind of performance one wants in a cache. 

### 2. Testing

I created a small testing function in `testing.c`, which gives a nice format for printing tests: It prints a bold green check mark next to tests that pass and a bold red ex-mark next to tests that fail.

My tests are pretty basic here, just making sure each of the functions are working. The cache resizing isn't really tested in these tests, because it's not really an exposed part of the API, but 

### 3. Performance

To improve performance, we will use a hash table for the cache. We will take a hash function  as a parameter in the creation of the cache, though for convienience we provide a simple default. 

### 4. Collision Resolution

I made the cache use a doubly-linked list for each bin in the hash table to handle collisions. This creates a little bit more complex code, but it avoids the cost of using the hash function again or having to linearly probe. 

### 5. Dynamic Resizing

The resizing is done by creating a new hash table double the size, taking each from the old hash table and putting it into a newly hashed position in the new table. This is a pretty expensive operation right now, though it may not happen that often. In the case of a hash, I would expect that the typical workload will grow the cache in the beginning, but woud keep the size stable for a while after. The cache can only grow; it never shrinks.

### 6. Eviction Policy

I implemented LRU as a simple linked list queue. The last recently used item is taken from the top of the queue and items can be bumped to the bottom of the queue. This uses O(n) space and getting is O(1), but it takes O(n) time to bump something to top of the queue. In hindsight, this isn't the greatest idea, because bumping—which needs to happen whenever we get from the cache—should be more common than getting—which happens whenever we need to evict. 

The upshot of this implementation is it is very modular, making it easy to test and switch out for anthor implementation without affecting the cache itself much.
