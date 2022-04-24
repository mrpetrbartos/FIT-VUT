# Makefile
# Řešení IJC-DU1, příklad a), 20.3.2022
# Autor: Petr Bartoš (xbarto0g), FIT VUT
# Přeloženo: gcc 11.1.0

CC=gcc
CFLAGS=-g -std=c11 -O2 -pedantic -Wall -Wextra

PRIMES=primes.o eratosthenes.o error.o bitset.o
PRIMES-I=primes-i.o eratosthenes-i.o bitset-i.o error.o
STEG=steg-decode.o ppm.o eratosthenes.o error.o

# COMPILE

all: primes primes-i steg-decode

run: all
	ulimit -s 40000 && ./primes && ./primes-i

primes: $(PRIMES)
	$(CC) $(CFLAGS) -o primes $(PRIMES) -lm

primes-i: $(PRIMES-I)
	$(CC) $(CFLAGS) -DUSE_INLINE -o primes-i $(PRIMES-I) -lm

steg-decode: $(STEG)
	$(CC) $(CFLAGS) -o steg-decode $(STEG) -lm

# OBJS

bitset.o: bitset.c bitset.h
	$(CC) $(CFLAGS) -c bitset.c -o bitset.o

primes.o: primes.c
	$(CC) $(CFLAGS) -c primes.c -o primes.o

eratosthenes.o: eratosthenes.c eratosthenes.h
	$(CC) $(CFLAGS) -c eratosthenes.c -o eratosthenes.o


bitset-i.o: bitset.c bitset.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE bitset.c -o bitset-i.o

primes-i.o: primes.c
	$(CC) $(CFLAGS) -c -DUSE_INLINE primes.c -o primes-i.o

eratosthenes-i.o: eratosthenes.c eratosthenes.h
	$(CC) $(CFLAGS) -c -DUSE_INLINE eratosthenes.c -o eratosthenes-i.o


error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o

steg-decode.o: steg-decode.c
	$(CC) $(CFLAGS) -c steg-decode.c -o steg-decode.o

ppm.o: ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c -o ppm.o	 

# TASKS

zip:
	zip xbarto0g.zip *.c *.h Makefile

clean:
	rm -f primes primes-i steg-decode *.o