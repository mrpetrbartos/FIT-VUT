// htab_free.c
// Řešení IJC-DU2, příklad b), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"
#include <stdlib.h>

void htab_free(htab_t *t)
{
    if (t == NULL)
        return;

    htab_clear(t);
    free(t->arr_ptr);
    free(t);
}
