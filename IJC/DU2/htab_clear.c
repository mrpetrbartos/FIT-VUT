// htab_clear.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include <stdlib.h>
#include "htab_hidden.h"

void htab_clear(htab_t *t)
{
    if (t == NULL)
        return;

    htab_item_t *curr, *next;

    for (size_t i = 0; i < t->arr_size; i++)
    {
        curr = t->arr_ptr[i];
        while (curr != NULL)
        {
            next = curr->next;
            free((char *)curr->pair.key);
            free(curr);
            curr = next;
        }
        t->arr_ptr[i] = NULL;
    }
    t->size = 0;
}
