// htab_for_each.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"

void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data))
{
    if (t == NULL)
        return;

    htab_item_t *curr;

    for (size_t i = 0; i < t->arr_size; i++)
    {
        curr = t->arr_ptr[i];
        while (curr != NULL)
        {
            f(&(curr->pair));
            curr = curr->next;
        }
    }
}
