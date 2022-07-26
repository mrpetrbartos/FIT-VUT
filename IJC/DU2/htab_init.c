// htab_init.c
// Řešení IJC-DU2, příklad b), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"
#include <stdlib.h>

htab_t *htab_init(size_t n)
{
    htab_t *table = malloc(sizeof(htab_t));
    if (table == NULL)
        return NULL;

    table->size = 0;
    table->arr_size = n;
    table->arr_ptr = malloc(sizeof(htab_item_t *) * n);
    if (table->arr_ptr == NULL)
    {
        free(table);
        return NULL;
    }

    for (size_t i = 0; i < n; i++)
    {
        table->arr_ptr[i] = NULL;
    }

    return table;
}
