// htab_size.c
// Řešení IJC-DU2, příklad b), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"

size_t htab_size(const htab_t *t)
{
    return t->size;
}
