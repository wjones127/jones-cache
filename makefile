#CFLAGS= -Wall -Werror -Wextra -Wpedantic -o3 -g
CFLAGS= -Wall -Wextra -Wpedantic -o3 -g

build_cache: src/cache.c
	gcc src/cache.c $(CFLAGS) -o build/cache

build/cache_test: src/cache.c src/test.c src/lru.c
	gcc src/test.c src/cache.c src/testing.c src/lru.c $(CFLAGS) -o build/cache_test

build/lru_test: src/lru.c src/lru_tests.c src/lru.h
	gcc src/lru.c src/lru_tests.c $(CFLAGS) -o build/lru_test

test_cache: build/cache_test
	build/cache_test

test_lru: build/lru_test
	build/lru_test
