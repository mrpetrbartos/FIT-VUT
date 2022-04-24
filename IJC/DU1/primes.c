// primes.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include <time.h>
#include <stdio.h>

#include "eratosthenes.h"
#include "error.h"

#define BITSET_SIZE 300000000

int main()
{
    clock_t start = clock();
    
    bitset_alloc(primes, BITSET_SIZE);
    Eratosthenes(primes);

    unsigned long primes_arr[10];
    int primes_count = 0;

    for (unsigned long iter = BITSET_SIZE - 1; iter >= 1; iter--)
    {
        if (!bitset_getbit(primes, iter))
            primes_arr[primes_count++] = iter;
        if (primes_count == 10) break;
    }

    for (int i = primes_count-1; i >= 0; i--)
    {
        printf("%lu\n", primes_arr[i]);
    }

    bitset_free(primes);
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC); 
}