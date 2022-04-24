// bitset.c
// Řešení IJC-DU1, příklad a), 20.3.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "bitset.h"

#ifdef USE_INLINE
extern inline void bitset_free(bitset_t jmeno_pole);
extern inline unsigned long bitset_size(bitset_t jmeno_pole);
extern void bitset_setbit(bitset_t jmeno_pole, const unsigned long index, const int vyraz);
extern inline unsigned long bitset_getbit(bitset_t jmeno_pole, const unsigned long index);    
#endif