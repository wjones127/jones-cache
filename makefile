CFLAGS= -Wall -Werror -Wextra -Wpedantic -o3 -g

build_cache: src/cache.c
	gcc src/cache.c $(CFLAGS) -o build/cache
