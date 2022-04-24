// eratosthenes.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "eratosthenes.h"
#include <math.h>

void Eratosthenes(bitset_t pole)
{
    bitset_setbit(pole, 0, 1);
    bitset_setbit(pole, 1, 1);
    unsigned long limit = sqrt(bitset_size(pole));

    for (unsigned long i = 2; i < bitset_size(pole); i += 2)
    {
        bitset_setbit(pole, i, 1);    
    }

    for (unsigned long i = 3; i < limit; i += 2)
    {
        if (!bitset_getbit(pole, i))
        {
            for (unsigned long j = i * i; j < bitset_size(pole); j += i)
            {
                bitset_setbit(pole, j, 1);
            }
        }
    }
}
