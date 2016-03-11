CFLAGS= -Wall -Werror -Wextra -Wpedantic -o3 -g

build_cache: src/cache.c
	gcc src/cache.c $(CFLAGS) -o build/cache

build_test: src/cache.c src/test.c
	gcc src/test.c src/cache.c -g -o build/cache_test

test: build/cache_test
	build/cache_test 256
