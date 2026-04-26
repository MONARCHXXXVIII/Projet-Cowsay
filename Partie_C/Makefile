CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: newcow wildcow reading_cow

newcow: newcow.c
	$(CC) $(CFLAGS) newcow.c -o newcow

wildcow: wildcow.c
	$(CC) $(CFLAGS) wildcow.c -o wildcow

reading_cow: reading_cow.c
	$(CC) $(CFLAGS) reading_cow.c -o reading_cow

clean:
	rm -f newcow.o wildcow.o reading_cow.o
