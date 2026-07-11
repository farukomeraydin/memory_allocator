CC?=cc
CFLAGS=-std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -pthread
all: test_allocator
test_allocator: mm.c test.c mm.h
	$(CC) $(CFLAGS) -o $@ mm.c test.c
sanitize:
	$(CC) $(CFLAGS) -fsanitize=address,undefined -g -o test_allocator mm.c test.c
clean:
	rm -f test_allocator
