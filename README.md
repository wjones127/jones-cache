# jones-cache
A generic networked look-aside cache. This is part of an assignment for Topics in Computer Systems at Reed College.

## Interface

The server-side code establishes an interface for the cache, which accepts the following types of requests:

- `GET /k`: Returns JSON `{ key: k, value: v }` where `k` is the requested key and `v` is corresponding value. Returns 404 if not found.
- `PUT /k/v`: creates or replaces entry with key `k` and value `v`.
- `DELETE /k`: deletes entry with key `k`. Responds with 200 status regardless of whether value was in cache.
- `HEAD /k`: Returns just a header, including HTTP version, Date, Accept, and Content-Type fields.
- `POST /shutdown`: Cleans up the cache and exits the server.
- `POST /memsize/value`: Creates a cache with `value` set as the maximum memory size of the cache. Returns 503 Error if a cache already exists.

There is also a C client api provided in `src/client.c`.

## Files

- `src/cache.c` implements cache
- `src/server.c` implements the server for the cache
- `src/client.c` implements networked cache client in C

## Implementation

This cache makes use of the [libevhtp](https://github.com/ellzey/libevhtp) library, which is a simple C library to create HTTP servers built upon [libevent](http://libevent.org/). 
