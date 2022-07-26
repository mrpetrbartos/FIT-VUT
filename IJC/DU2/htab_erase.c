// htab_erase.c
// Řešení IJC-DU2, příklad a), 10.04.2022
// Autor: Petr Bartoš (xbarto0g), FIT VUT
// Přeloženo: gcc 11.1.0

#include "htab_hidden.h"
#include <stdlib.h>

bool htab_erase(htab_t *t, htab_key_t key)
{
    if (t == NULL)
        return false;

    size_t index = htab_hash_function(key) % t->arr_size;
    htab_item_t *curr = t->arr_ptr[index];
    htab_item_t *prev = NULL;

    while (curr != NULL)
    {
        if (!strcmp(key, curr->pair.key))
        {
            if (prev != NULL)
                prev->next = curr->next;
            else
                t->arr_ptr[index] = curr->next;

            free((char *)curr->pair.key);
            free(curr);
            t->size--;

            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    if (((float)t->size / t->arr_size) < AVG_LEN_MIN)
        htab_resize(t, (size_t)((float)t->arr_size / 2));

    return false;
}
