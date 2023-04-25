CC=gcc
CFFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
RM=rm -f

proj2 : proj2.c
		$(CC) $(CFFLAGS) proj2.c -o proj2
	
clean : $(RM) *.o proj2