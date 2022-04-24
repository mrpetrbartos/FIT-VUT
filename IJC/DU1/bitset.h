// bitset.h
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#ifndef BITSET_H
#define BITSET_H

#include "error.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

typedef unsigned long bitset_t[];

typedef unsigned long bitset_index_t;

#define BITS_IN_UL (sizeof(unsigned long) * CHAR_BIT)

#define bitset_create(jmeno_pole, velikost) \
    static_assert(velikost > 0, "Velikost pole musi byt kladna"); \
    unsigned long jmeno_pole[1 + velikost / BITS_IN_UL + ((velikost % BITS_IN_UL) ? 1 : 0)] = {velikost};

#define bitset_alloc(jmeno_pole, velikost) \
    assert(velikost > 0); \
    unsigned long *jmeno_pole = calloc((1 + velikost / BITS_IN_UL + ((velikost % BITS_IN_UL) ? 1 : 0)), sizeof(unsigned long)); \
    if (jmeno_pole == NULL) \
    { \
        error_exit("bitset_alloc: Chyba alokace paměti\n"); \
    } \
    *jmeno_pole = velikost;


#ifndef USE_INLINE
    #define bitset_free(jmeno_pole) free(jmeno_pole)

    #define bitset_size(jmeno_pole) jmeno_pole[0]

    #define bitset_setbit(jmeno_pole, index, vyraz) \
        if (index >= jmeno_pole[0]) \
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long)index, (unsigned long)(jmeno_pole[0]-1)); \
        vyraz != 0 ? (jmeno_pole[1 + ((unsigned long)index) / BITS_IN_UL] |= (1UL << (unsigned long)index % BITS_IN_UL)) \
                    : (jmeno_pole[1 + ((unsigned long)index) / BITS_IN_UL] &= ~(1UL << (((unsigned long)index)% BITS_IN_UL)))

    #define bitset_getbit(jmeno_pole, index) \
        (index >= jmeno_pole[0] ? \
        error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long)index, (unsigned long)(jmeno_pole[0]-1)), 0 : \
        jmeno_pole[1 + (unsigned long)index / BITS_IN_UL] & (1UL << (unsigned long)index % BITS_IN_UL))
#else
    inline void bitset_free(bitset_t jmeno_pole)
    {
        free(jmeno_pole);
    }    

    inline unsigned long bitset_size(bitset_t jmeno_pole)
    {
        return jmeno_pole[0];
    }

    inline void bitset_setbit(bitset_t jmeno_pole, const unsigned long index, const int vyraz)
    {
        if (index >= jmeno_pole[0])
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long)index, (unsigned long)(jmeno_pole[0]-1));
        vyraz != 0 ? (jmeno_pole[1 + (unsigned long)index / BITS_IN_UL] |= (1UL << (unsigned long)index % BITS_IN_UL))
                    : (jmeno_pole[1 + (unsigned long)index / BITS_IN_UL] &= ~(1UL << (unsigned long)index % BITS_IN_UL));
        return;        
    }

    inline unsigned long bitset_getbit(bitset_t jmeno_pole, const unsigned long index)
    {
        if (index >= jmeno_pole[0])
            error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu\n", (unsigned long)index, (unsigned long)(jmeno_pole[0]-1));
        return(jmeno_pole[1 + (unsigned long)index / BITS_IN_UL] & (1UL << (unsigned long)index % BITS_IN_UL));    
    }
#endif

#endif