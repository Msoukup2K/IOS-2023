CC=gcc
CFFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
binaries= proj2 proj2.out

.PHONY: clean all

all : proj2.c
		$(CC) $(CFFLAGS) proj2.c -o proj2
	
clean :  proj2
		rm -f $(binaries) *.o