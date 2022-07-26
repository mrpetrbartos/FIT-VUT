# Makefile
# Řešení IJC-DU2, příklad a), 10.04.2022
# Autor: Petr Bartoš (xbarto0g), FIT VUT
# Přeloženo: gcc 11.1.0

CC=gcc
CFLAGS=-g -std=c11 -O2 -pedantic -Wall -Wextra

OBJS=htab_bucket_count.o htab_clear.o htab_erase.o htab_find.o htab_for_each.o htab_free.o htab_hash_function.o htab_init.o htab_lookup_add.o htab_resize.o htab_size.o


# COMPILE

all: libhtab.a libhtab.so wordcount wordcount-dynamic tail

tail: tail.o
	$(CC) $(CFLAGS) -o tail tail.o -lm

libhtab.a: $(OBJS)
	ar rcs $@ $^

libhtab.so: $(OBJS)
	$(CC) -fPIC -shared $^ -o $@

wordcount: wordcount.o io.o libhtab.a
	$(CC) $(CFLAGS) -static $^ -o $@

wordcount-dynamic: wordcount.o io.o libhtab.so
	$(CC) $(CFLAGS) -L. $^ -o $@

# OBJS

%.o : %.c htab.h htab_struct.h
	$(CC) $(CFLAGS) -fPIC -c $<

# TASKS

zip:
	zip xbarto0g.zip *.c *.h Makefile

clean:
	rm -f libhtab.a libhtab.so wordcount wordcount-dynamic tail *.o
